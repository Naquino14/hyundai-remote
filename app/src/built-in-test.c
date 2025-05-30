#include "built-in-test.h"
#include "roles.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/linker/linker-defs.h>
#include <zephyr/display/cfb.h>
#include <zephyr/drivers/gnss.h>
#include <zephyr/drivers/uart.h>

static const char* HASHES = "################################";

// get led node from dt
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#if defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_FOB)
#define DISPLAY_NODE DT_NODELABEL(ssd1306)
static const struct device *display = DEVICE_DT_GET(DISPLAY_NODE);

#define GPS_NODE NULL
static const struct device *gps_modem = NULL;

#elif defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_TRC)
#define DISPLAY_NODE DT_NODELABEL(st7735)
static const struct device *display = DEVICE_DT_GET(DISPLAY_NODE);

#define GPS_MODEM_DEV DEVICE_DT_GET(DT_ALIAS(gps))
static const struct device *const gps_modem = GPS_MODEM_DEV;

#define GPS_UART DEVICE_DT_GET(DT_NODELABEL(uart1))
static const struct device *gps_uart = GPS_UART;
#endif

LOG_MODULE_REGISTER(bit, LOG_LEVEL_DBG);

static int bit_display_clear() {
    int ret = cfb_framebuffer_clear(display, true);
    if (ret != 0) {
        printk("CFB clear failed: %d\n", ret);
        return ret;
    }
    return 0;
}

static int bit_display_text(const char* text) {
    return cfb_print(display, text, 0, 0);
}

static int bit_display_textxy(const char* text, int x, int y) {
    int ret = cfb_print(display, text, 0, 0);
    if (ret != 0) {
        printk("CFB print failed: %d\n", ret);
        return ret;
    }
    return 0;
}

static int bit_display_flush() {
    int ret = cfb_framebuffer_finalize(display);
    if (ret != 0) {
        printk("CFB finalize failed: %d\n", ret);
        return ret;
    }
    return 0;
}

static void gnss_data_cb(const struct device *dev, const struct gnss_data *data) {
    uint64_t timepulse_ns;
    k_ticks_t timepulse;

    if (data->info.fix_status != GNSS_FIX_STATUS_NO_FIX) {
        if (gnss_get_latest_timepulse(dev, &timepulse) == 0) {
            timepulse_ns = k_ticks_to_ns_near64(timepulse);
            printk("Got a fix @ %lld ns\n", timepulse_ns);
        } else {
            printk("Got a fix, but no timepulse\n");
        }
    }
}
GNSS_DATA_CALLBACK_DEFINE(GPS_MODEM_DEV, gnss_data_cb);

static void gnss_satellites_cb(const struct device *dev, 
                                const struct gnss_satellite *satellies, 
                                uint16_t num_satellites) {
        unsigned int tracked = 0;

        for (unsigned int i = 0; i != num_satellites; i++) 
            tracked += (satellies[i].is_tracked ? 1 : 0);
        
    printk("Of %u satellites, %u are tracked\n", num_satellites, tracked);
}
GNSS_SATELLITES_CALLBACK_DEFINE(GPS_MODEM_DEV, gnss_satellites_cb);

#define GNSS_SYSTEMS_PRINTF(define, supported, enabled)                                        \
	printf("\t%20s: Supported: %3s | Enabled: %3s\n",                                          \
	       STRINGIFY(define), (supported & define) ? "Yes" : "No",                             \
			 (enabled & define) ? "Yes" : "No");

void read_gnss_uart_thread() {
    const char* clear_command = "$CFGCLR\r\n";
    // int ret = uart_tx(gps_uart, clear_command, strlen(clear_command), SYS_FOREVER_MS);
    // if (ret != 0) {
    //     printk("Failed to send clear command: %d\n", ret);
    //     return;
    // }

    for (int i = 0; i < strlen(clear_command); i++) {
        uint8_t c = clear_command[i];
        int ret = uart_tx(gps_uart, &c, 1, SYS_FOREVER_MS);
        if (ret != 0) {
            printk("Failed to send char: %d\n", ret);
            return;
        }
    }

    for (;;) {
        uint8_t c;
        int ret = uart_poll_in(gps_uart, &c);
        if (ret == 0) 
            printk("%c", c);
        else
            k_msleep(10);
    }
}

void run_bit() {
    const char* startup_text = "Waking up...\n";
    
    if (display) {
        // cfb driver
        if (!device_is_ready(display)) {
            printk("Display device not ready\n");
        }

        int ret = cfb_framebuffer_init(display);
        if (ret != 0) {
            printk("Display init failed: %d\n", ret);
        }

        bit_display_text(startup_text);
        bit_display_flush();
    }


    printk("%s\n", startup_text);

    k_msleep(2 * 1000);

    printk("%s\n", HASHES);
    printk("#      HYUNDAI-REMOTE BIT      #\n");
    printk("# Board: %-21s #\n", CONFIG_BOARD);
    printk("# Role: %-22s #\n", role_tostring());
    printk("%s\n", HASHES);

    if (display) {
        bit_display_clear();
        bit_display_textxy(*role_tostring() == 'F' ? "ROLE: FOB" : "ROLE: TRC", 0, 0);
        bit_display_flush();
    }

    // gps setup
    if (gps_uart) {
        if (!device_is_ready(gps_uart)) 
            printk("GPS UART dev not ready\n");
    }

    if (gps_modem) {
        printk("########### GNSS BIT ###########\nSystems:\n");
        do {
            if (!device_is_ready(gps_modem)) {
                printk("GNSS device not ready\n");
                break;
            }

            gnss_systems_t supported, enabled;
            uint32_t fix_rate;
            
            int ret = gnss_get_supported_systems(gps_modem, &supported);
            if (ret != 0) {
                printk("Failed to get supported systems: %d\n", ret);
                break;
            }
            
            ret = gnss_get_enabled_systems(gps_modem, &enabled);
            if (ret != 0) {
                printk("Failed to get enabled systems: %d\n", ret);
                break;
            }
            
            GNSS_SYSTEMS_PRINTF(GNSS_SYSTEM_GPS, supported, enabled);
	        GNSS_SYSTEMS_PRINTF(GNSS_SYSTEM_GLONASS, supported, enabled);
	        GNSS_SYSTEMS_PRINTF(GNSS_SYSTEM_GALILEO, supported, enabled);
	        GNSS_SYSTEMS_PRINTF(GNSS_SYSTEM_BEIDOU, supported, enabled);
	        GNSS_SYSTEMS_PRINTF(GNSS_SYSTEM_QZSS, supported, enabled);
	        GNSS_SYSTEMS_PRINTF(GNSS_SYSTEM_IRNSS, supported, enabled);
	        GNSS_SYSTEMS_PRINTF(GNSS_SYSTEM_SBAS, supported, enabled);
	        GNSS_SYSTEMS_PRINTF(GNSS_SYSTEM_IMES, supported, enabled);
            
            ret = gnss_get_fix_rate(gps_modem, &fix_rate);

            if (ret != 0) {
                printk("Failed to get fix interval: %d\n", ret);
                break;
            }
            else
                printk("Please set fix interval: %u ms\n", fix_rate);
        } while (false);

        printk("######### END GNSS BIT #########\n\n");   
    }

    // static bool state = true;

    for (;;) {
        /*int ret = */gpio_pin_toggle_dt(&led);
        // int read_state = gpio_pin_get_dt(&led);
        // printk("Hello World! %s\tState: %s\tRead State: %s\t", CONFIG_BOARD, state ? "ON " : "OFF", read_state == GPIO_OUTPUT_HIGH ? "HI" : "LO");
        // printk("%s%d\n\n", ret == 0 ? "    OK: " : "NOT OK: ", ret);

        // state = !state;

        k_msleep(500);
    }
}

void stop_bit() {

}