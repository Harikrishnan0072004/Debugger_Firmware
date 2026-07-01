// this for sram , after the 3s into jump to bootsel mode.
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"

int main()
{
    stdio_init_all();

    printf("Running from SRAM!\n");

    sleep_ms(3000);

    // Jump to BOOTSEL mode
    reset_usb_boot(0, 0);  //calling the bootrom

    while (1);
}