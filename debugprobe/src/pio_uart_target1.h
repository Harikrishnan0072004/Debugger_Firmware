#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void target1_pio_uart_init(void);
void target1_pio_uart_write_byte(uint8_t ch);
void target1_pio_uart_write(const uint8_t *data, size_t len);
bool target1_pio_uart_read_byte(uint8_t *ch);
void target1_pio_uart_usb_task(uint8_t cdc_itf);