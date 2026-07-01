#include<stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT  i2c0
#define SDA_PIN 6
#define SCL_PIN 7
#define SLAVE_ADD 0x17

int main(){
stdio_init_all();
gpio_init(25);
gpio_set_dir(25 , GPIO_OUT);
gpio_put(25 , 1);
printf("I2C SLAVE start \n");
printf("my address is 0x%02x\n", SLAVE_ADD);

//INIt I2c at 100 mkz
// i2c_init(I2C_PORT , 100*1000);
i2c_init(I2C_PORT, 1000 * 1000);
//CONNECT GP4 AND GP5 TO I2C  hardware
gpio_set_function(SDA_PIN , GPIO_FUNC_I2C);
gpio_set_function(SCL_PIN , GPIO_FUNC_I2C);


//I2C needs pull_up register. internal pull-up is okey for first lab.
gpio_pull_up(SDA_PIN);
gpio_pull_up(SCL_PIN);
//make this pico an I2C slave  with address 0x17
 i2c_set_slave_mode(I2C_PORT , true ,SLAVE_ADD);

while (1){
   
   uint8_t rx_data = 0 ; 
   //wait until master writes 1 byte to this slave
   i2c_read_raw_blocking(I2C_PORT , &rx_data ,1 );
   printf("slave received: 0x%02x\n ", rx_data);
  
 }

}