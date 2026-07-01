//exit xip and enter xip again
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"


int main(void)
{
    stdio_init_all();

    gpio_init(25);
gpio_set_dir(25, GPIO_OUT);
gpio_put(25 ,1);
    printf("Running from SRAM\n");

    sleep_ms(2000);

    printf("Exit XIP\n");

     rom_flash_exit_xip();

    sleep_ms(2000);

    printf("Enter XIP\n");

    rom_flash_enter_cmd_xip();

    rom_flash_flush_cache();
gpio_put(25 ,0);
    while (1)
    {
        tight_loop_contents();
    }
}