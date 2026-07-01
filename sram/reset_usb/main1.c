//exit xip and enter xip again
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"

int main(void)
{
    stdio_init_all();

    printf("Running from SRAM\n");

    sleep_ms(2000);

    printf("Exit XIP\n");

    flash_exit_xip();

    sleep_ms(2000);

    printf("Enter XIP\n");

    flash_enter_cmd_xip();

    flash_flush_cache();

    while (1)
    {
        tight_loop_contents();
    }
}