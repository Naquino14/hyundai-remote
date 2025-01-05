#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/linker/linker-defs.h>

int main(void) {
    while (1) {
        printf("Hello World! %s\n", CONFIG_BOARD);
        printf("Address of sample %p\n", (void *)__rom_region_start);
        fflush(stdout);
        k_msleep(500);
    }
    return 0;
}