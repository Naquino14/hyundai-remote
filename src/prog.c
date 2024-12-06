#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#define LED_WHITE GPIO_NUM_35
#define BUTTON_PROG GPIO_NUM_0

static TaskHandle_t blink_handle = NULL;
static bool state = false;

void IRAM_ATTR button_isr(void *arg) {
    if (blink_handle != NULL) {
        (eTaskGetState(blink_handle) == eSuspended
             ? vTaskResume
             : vTaskSuspend)(blink_handle);
    }
}

void blink_task(void *pvParams) {
    int delay = *((int *)pvParams);

    for (;;) {
        gpio_set_level(LED_WHITE, state);
        printf("Hello world!\n");
        state = !state;
        vTaskDelay(pdMS_TO_TICKS(delay));
    }
}

void app_main(void) {
    gpio_config_t prog_btn_conf = {
        .pin_bit_mask = (1ull << BUTTON_PROG),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,  // falling edge
    };
    ESP_ERROR_CHECK(gpio_config(&prog_btn_conf));

    gpio_config_t io_config = {
        .pin_bit_mask = (1ull << LED_WHITE),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&io_config));

    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_PROG, button_isr, NULL));

    setvbuf(stdout, NULL, _IONBF, 0);
    int delayMs = 500;
    xTaskCreate(blink_task, "Blinker", 2048, (void *)&delayMs, 5, &blink_handle);
}
