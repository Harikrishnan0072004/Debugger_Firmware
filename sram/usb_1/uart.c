#include "pico/stdlib.h"

int main(void)
{
    const uint LED = PICO_DEFAULT_LED_PIN;

    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);
printf("hellow");
    while (1)
    {
        gpio_put(LED, 1);
        sleep_ms(5000);

        gpio_put(LED, 0);
        sleep_ms(5000);
    }
}