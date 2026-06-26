#include "pio_uart_target1.h"

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "pio_uart_target2.h"
#include "pio_uart_target1.pio.h"

#define TARGET2_UART_PIO      pio1 //PIO 1
#define TARGET2_UART_TX_SM    2
#define TARGET2_UART_RX_SM    3
#define TARGET2_UART_TX_PIN   12   // Debugger TX -> Target RX
#define TARGET2_UART_RX_PIN   13   // Debugger RX <- Target TX
#define TARGET2_BAUD          9600

static PIO s_pio = TARGET2_UART_PIO;
static uint s_sm_tx = TARGET2_UART_TX_SM;
static uint s_sm_rx = TARGET2_UART_RX_SM;
#include "tusb.h"

void target2_pio_uart_usb_task(uint8_t cdc_itf) {
    uint8_t buf[64];

    // PC USB CDC -> PIO UART TX -> target RX
    if (tud_cdc_n_connected(cdc_itf)) {
        uint32_t count = tud_cdc_n_available(cdc_itf);

        if (count > sizeof(buf)) {
            count = sizeof(buf);
        }

        if (count > 0) {
            uint32_t n = tud_cdc_n_read(cdc_itf, buf, count);
            target2_pio_uart_write(buf, n);
        }
    }

    // target TX -> PIO UART RX -> PC USB CDC
    if (tud_cdc_n_connected(cdc_itf)) {
        while (tud_cdc_n_write_available(cdc_itf)) {
            uint8_t ch;

            if (!target2_pio_uart_read_byte(&ch)) {
                break;
            }

            tud_cdc_n_write_char(cdc_itf, ch);
        }

        tud_cdc_n_write_flush(cdc_itf);
    }
}
void target2_pio_uart_init(void) {
    // Reserve fixed state machines.
    // Do not use pio_claim_free_sm() inside debugger firmware.
    if (!pio_sm_is_claimed(s_pio, s_sm_tx)) {
        pio_sm_claim(s_pio, s_sm_tx);
    }

    if (!pio_sm_is_claimed(s_pio, s_sm_rx)) {
        pio_sm_claim(s_pio, s_sm_rx);
    }

    // Add PIO programs after existing SWD program is loaded.
    hard_assert(pio_can_add_program(s_pio, &uart_tx_program));
    uint offset_tx = pio_add_program(s_pio, &uart_tx_program);

    hard_assert(pio_can_add_program(s_pio, &uart_rx_program));
    uint offset_rx = pio_add_program(s_pio, &uart_rx_program);

    uart_tx_program_init(
        s_pio,
        s_sm_tx,
        offset_tx,
        TARGET2_UART_TX_PIN,
        TARGET2_BAUD
    );

    uart_rx_program_init(
        s_pio,
        s_sm_rx,
        offset_rx,
        TARGET2_UART_RX_PIN,
        TARGET2_BAUD
    );
}

void target2_pio_uart_write_byte(uint8_t ch) {
    uart_tx_program_putc(s_pio, s_sm_tx, (char)ch);
}

void target2_pio_uart_write(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        target2_pio_uart_write_byte(data[i]);
    }
}

bool target2_pio_uart_read_byte(uint8_t *ch) {
    if (pio_sm_is_rx_fifo_empty(s_pio, s_sm_rx)) {
        return false;
    }

    *ch = (uint8_t)uart_rx_program_getc(s_pio, s_sm_rx);
    return true;
}