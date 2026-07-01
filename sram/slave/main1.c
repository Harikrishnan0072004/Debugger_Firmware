#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/i2c_slave.h"

#define I2C_PORT i2c1
#define SDA_PIN 6
#define SCL_PIN 7
#define SLAVE_ADDR 0x42

void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    static char buffer[64];
    static int index = 0;

    switch(event)
    {
        case I2C_SLAVE_RECEIVE:
        {
            uint8_t ch = i2c_read_byte_raw(i2c);

            buffer[index++] = ch;

            if(index >= sizeof(buffer)-1)
                index = 0;

            break;
        }

        case I2C_SLAVE_FINISH:
        {
            buffer[index] = '\0';

            printf("Received: %s\n", buffer);

            index = 0;

            break;
        }

        default:
            break;
    }
}

int main()
{
    stdio_init_all();

    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    i2c_slave_init(
        I2C_PORT,
        SLAVE_ADDR,
        &i2c_slave_handler
    );

    while (true)
    {
        tight_loop_contents();
    }
}