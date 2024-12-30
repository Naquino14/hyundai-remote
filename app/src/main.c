#include <stdio.h>
#include <zephyr/kernel.h>

int main(void) {
    for (;;) {
        printf("Hello World! %s\n", CONFIG_BOARD);
        k_msleep(500);

        return 0;
    }
}