#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <unistd.h>

int main(void)
{
    for (;;)
    {
        printk("Hello World! This is Zephyr on ESP32-S3 DevKitM.\n");
        k_msleep(500);
    }
    return 0;
}
