#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "pico/bootrom.h"

static uint8_t flash_buf[16];

void __not_in_flash_func(read_flash_test)(void) {
    uint32_t irq = save_and_disable_interrupts();

    rom_flash_exit_xip();

    const uint8_t *flash_ptr = (const uint8_t *)0x10000000;
    for (int i = 0; i < 16; i++) {
        flash_buf[i] = flash_ptr[i];
    }

    rom_flash_enter_cmd_xip();

    restore_interrupts(irq);
}

int main() {
    stdio_init_all();
    sleep_ms(1000);

    read_flash_test();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    while (true) {
        gpio_put(25, 1);
        sleep_ms(200);
        gpio_put(25, 0);
        sleep_ms(200);
    }
}