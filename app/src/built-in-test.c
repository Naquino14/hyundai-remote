#include "built-in-test.h"
#include "roles.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/linker/linker-defs.h>
#include <zephyr/display/cfb.h>
#include <zephyr/drivers/gnss.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/lora.h>

static const char* HASHES = "################################";

// get led node from dt
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec sw0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static struct gpio_callback sw0_cb_data;

#if defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_FOB)
#define DISPLAY_NODE DT_NODELABEL(ssd1306)
static const struct device *display = DEVICE_DT_GET(DISPLAY_NODE);

#define LORA_NODE DT_NODELABEL(lora0)
static const struct device *lora = DEVICE_DT_GET(LORA_NODE);
#define LORA_MAX_POW_DBM 14

#elif defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_TRC)
#define DISPLAY_NODE DT_NODELABEL(st7735)
static const struct device *display = DEVICE_DT_GET(DISPLAY_NODE);

#define LORA_MAX_POW_DBM 18 // up to 21±1
#define LORA_NODE DT_ALIAS(lora0)
static const struct device *lora = DEVICE_DT_GET(LORA_NODE);
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

static bool sw0_ok = false;
static void button_pressed(const struct device* dev, struct gpio_callback *cb, uint32_t pins) {
    LOG_INF("SW0 OK.");
    sw0_ok = true;
}

static struct lora_modem_config lora_cfg = {
    .frequency = MHZ(915),
    .bandwidth = BW_125_KHZ,
    .datarate = SF_10,
    .preamble_len = 8,
    .coding_rate = CR_4_5,
    .iq_inverted = false,
    .public_network = false,
    .tx_power = LORA_MAX_POW_DBM
};

static bool do_pong = false, listening = true;
static void lora_rx_cb(const struct device *dev, uint8_t* data, uint16_t len, int16_t rssi, int8_t snr, void* user_data) {
    LOG_INF("Lora Packet Rx: %.*s, RSSI: %d, SNR: %d", len, data, rssi, snr);
    do_pong = true;
}

void run_bit() {
    const char* startup_text = "Waking up...\n";

    int ret;
    
    if (display) {
        // cfb driver
        if (!device_is_ready(display)) {
            printk("Display device not ready\n");
        }

        ret = cfb_framebuffer_init(display);
        if (ret != 0) {
            printk("Display init failed: %d\n", ret);
        }

        bit_display_clear();
        bit_display_text(startup_text);
        bit_display_flush();
    }

    if (sw0.port) {
        gpio_init_callback(&sw0_cb_data, button_pressed, BIT(sw0.pin));
        ret = gpio_add_callback(sw0.port, &sw0_cb_data);
        if (ret < 0) 
            printk("Failed to register SW0 callback: %d\n", ret);
    }

    printk("%s\n", startup_text);
    
    k_msleep(2 * 1000);

    if (lora) do {
                if (!device_is_ready(lora)) 
                    printk("Lora device is not ready\n");
        
                lora_cfg.tx = (role_get() == ROLE_FOB);

                ret = lora_config(lora, &lora_cfg);
                if (ret < 0) {
                    printk("Lora config failed: %d\n", ret);
                    break;
                }
                printk("Lora OK\n");

                if (lora && role_get() == ROLE_TRC) {
                    printk("Registering Lora Rx callback...\n");
                    lora_recv_async(lora, lora_rx_cb, NULL);
                    listening = true;
                }
            } while (false);

    printk("%s\n", HASHES);
    printk("#      HYUNDAI-REMOTE BIT      #\n");
    printk("# Board: %-21s #\n", CONFIG_BOARD);
    printk("# Role: %-22s #\n", role_tostring());
    printk("%s\n", HASHES);

    if (display) {
        bit_display_clear();
        bit_display_text(*role_tostring() == 'F' ? "ROLE: FOB   PRESS SW0" : "ROLE: TRC   PRESS SW0");
        bit_display_flush();
    }

    // static bool state = true;

    for (;;) {
        /*int ret = */gpio_pin_toggle_dt(&led);
        // int read_state = gpio_pin_get_dt(&led);
        // printk("Hello World! %s\tState: %s\tRead State: %s\t", CONFIG_BOARD, state ? "ON " : "OFF", read_state == GPIO_OUTPUT_HIGH ? "HI" : "LO");
        // printk("%s%d\n\n", ret == 0 ? "    OK: " : "NOT OK: ", ret);

        // state = !state;
        if (sw0_ok && sw0.port) {
            sw0_ok = false;
            bit_display_clear();
            bit_display_text("SW0 PRESSED");
            bit_display_flush();

            if (lora && role_get() == ROLE_FOB) {
                // send ping
                char data[] = "PING";
                printk("Pinging TRC...\n");
                ret = lora_send(lora, data, sizeof(data));
                if (ret < 0) {
                    printk("Lora send failed: %d\n", ret);
                }

                // listen for pong
                int16_t rssi;
                uint8_t snr;
                ret = lora_recv(lora, data, sizeof(data), K_MSEC(10000), &rssi, &snr);
                if (ret < 0) {
                    printk("Lora recv failed: %d\n", ret);
                } else {
                    printk("PONG received: %s, RSSI: %d, SNR: %d\n", data, rssi, snr);
                    bit_display_clear();
                    bit_display_text("PONG");
                    bit_display_flush();
                }
            }
        }

        if (lora && do_pong) do {
                printk("Pinged, ponging...\n");

                // stop rx
                lora_recv_async(lora, NULL, NULL);
                listening = false;
                
                lora_cfg.tx = true; 
                int ret = lora_config(lora, &lora_cfg);
                if (ret < 0) {
                    printk("Lora rx cb: Set Lora cfg TX failed: %d\n", ret);
                    break;
                }

                ret = lora_send(lora, "PONG", 4);
                if (ret < 0) {
                    printk("Lora send failed: %d\n", ret);
                    break;
                } else 
                    printk("PONG sent successfully.\n");

                k_msleep(100);
                lora_cfg.tx = false; 
                ret = lora_config(lora, &lora_cfg);
                if (ret < 0) 
                    printk("Lora rx cb: Set Lora cfg RX failed: %d\n", ret);
                
                do_pong = false;
            } while (do_pong);

        if (!do_pong && !listening) {
            lora_recv_async(lora, lora_rx_cb, NULL);
            listening = true;
        }

        k_msleep(500);
    }
}

void stop_bit() {

}