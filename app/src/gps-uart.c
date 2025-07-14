#include "gps-uart.h"
#include "roles.h"

#if defined(CONFIG_DEVICE_ROLE) && (CONFIG_DEVICE_ROLE == DEF_ROLE_TRC)

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <string.h>

LOG_MODULE_REGISTER(gps_uart, LOG_LEVEL_DBG);

#define GPS_UART_NODE DT_NODELABEL(uart1)

static const struct device* gps_uart_dev = DEVICE_DT_GET(GPS_UART_NODE);

#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec sw0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

static void gps_uart_cb(const struct device *dev, void *user_data) {
    ARG_UNUSED(user_data);

    while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
        if (uart_irq_rx_ready(dev)) {
            uint8_t c;
            if (uart_fifo_read(dev, &c, 1)) {
                printk("%c", c);
            }
        }
    }
}

void send_buf(const char* cmd) {
    while (*cmd)
        uart_poll_out(gps_uart_dev, *cmd++);
    
    uart_poll_out(gps_uart_dev, '\r');
    uart_poll_out(gps_uart_dev, '\n');
}

void begin_gps_uart() {
    k_msleep(500);

    const struct device *gpio0;
    gpio0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));

    // code to listen and talk to GPS UART here
    LOG_INF("GPS UART initialized");

    if (!device_is_ready(gps_uart_dev)) {
        LOG_ERR("GPS UART DEV not OK");
    }
    else
        LOG_INF("GPS UART DEV OK");

    uart_irq_callback_user_data_set(gps_uart_dev, gps_uart_cb, NULL);
    uart_irq_rx_enable(gps_uart_dev);
    
    k_msleep(100);

    // ping gps to see if its alive
    send_buf("$PDTINFO");
    printk("***\n");
    send_buf("$PMTK605");

    LOG_INF("CMD Sequence done.");

    while (!gpio_pin_get_dt(&sw0))
        k_msleep(1);
} 

#endif