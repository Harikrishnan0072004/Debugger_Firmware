#include "hardware/regs/io_bank0.h"
#include "hardware/regs/pads_bank0.h"
#include "hardware/regs/sio.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/clocks.h"
#include "hardware/structs/io_bank0.h"
#include "hardware/structs/pads_bank0.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/resets.h"
#include "pico/bootrom.h"
#include "hardware/sync.h"

// Simple software delay
static void busy_delay(volatile uint32_t count) {
    while (count--) {
        __asm volatile ("nop");
    }
}

// RAM-resident function
void __not_in_flash_func(ram_main)(void) {
    uint32_t irq = save_and_disable_interrupts();

    // Turn off XIP
    rom_flash_exit_xip();

    // Enable GPIO (reset/io not strictly needed in ROM tiny tests, but ok)
    // Configure GPIO25 as output using SIO and IO_BANK0 directly

    // Select function SIO for GPIO25
    io_bank0_hw->io[25].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;

    // Set pad: enable output
    pads_bank0_hw->io[25] = PADS_BANK0_GPIO0_OD_LSB; // simple: default + output disable off

    // Set direction output
    sio_hw->gpio_oe_set = (1u << 25);

    // Main loop: blink forever from RAM
    while (1) {
        // LED on
        sio_hw->gpio_set = (1u << 25);
        busy_delay(500000);

        // LED off
        sio_hw->gpio_clr = (1u << 25);
        busy_delay(500000);
    }

    // We never get here, but for completeness:
    rom_flash_enter_cmd_xip();
    restore_interrupts(irq);
}

// Main just jumps into RAM function.
// Mark main itself not_in_flash_func if you want, or
// build with PICO_COPY_TO_RAM=1 in CMake.
int main(void) {
    ram_main();
    while (1) {
        // should never reach here
    }
}