#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "pio_uart_target1.pio.h"

#define TARGET1_UART_PIO      pio0
#define TARGET1_UART_TX_SM    2
#define TARGET1_UART_RX_SM    3
#define TARGET1_UART_TX_PIN   0   // Debugger TX -> Target RX
#define TARGET1_UART_RX_PIN   1   // Debugger RX <- Target TX
#define TARGET1_BAUD          115200

int main() {
    stdio_init_all();

    sleep_ms(2000);
    printf("Target1 PIO UART bridge started\r\n");

    PIO pio_uart = TARGET1_UART_PIO;

    uint sm_tx = TARGET1_UART_TX_SM;
    uint sm_rx = TARGET1_UART_RX_SM;

    // Check and reserve fixed state machines.
    hard_assert(!pio_sm_is_claimed(pio_uart, sm_tx));
    hard_assert(!pio_sm_is_claimed(pio_uart, sm_rx));

    pio_sm_claim(pio_uart, sm_tx);
    pio_sm_claim(pio_uart, sm_rx);

    // Add TX and RX PIO programs into PIO0 instruction memory.
    hard_assert(pio_can_add_program(pio_uart, &uart_tx_program));
    uint offset_tx = pio_add_program(pio_uart, &uart_tx_program);

    hard_assert(pio_can_add_program(pio_uart, &uart_rx_program));
    uint offset_rx = pio_add_program(pio_uart, &uart_rx_program);

    // Initialize PIO UART TX on GP0.
    uart_tx_program_init(
        pio_uart,
        sm_tx,
        offset_tx,
        TARGET1_UART_TX_PIN,
        TARGET1_BAUD
    );

    // Initialize PIO UART RX on GP1.
    uart_rx_program_init(
        pio_uart,
        sm_rx,
        offset_rx,
        TARGET1_UART_RX_PIN,
        TARGET1_BAUD
    );

    printf("PIO UART ready: TX GP%d, RX GP%d\r\n",
           TARGET1_UART_TX_PIN,
           TARGET1_UART_RX_PIN);

    uart_tx_program_puts(
        pio_uart,
        sm_tx,
        "Hello Target1, this is PIO UART\r\n"
    );

    while (true) {
        // Target1 UART TX -> Debugger GP1 -> USB terminal
        while (!pio_sm_is_rx_fifo_empty(pio_uart, sm_rx)) {
            char c = uart_rx_program_getc(pio_uart, sm_rx);
            putchar(c);
        }

        // USB terminal -> Debugger GP0 -> Target1 UART RX
        int ch = getchar_timeout_us(0);

        if (ch != PICO_ERROR_TIMEOUT) {
            uart_tx_program_putc(pio_uart, sm_tx, (char)ch);
        }

        tight_loop_contents();
    }
}