#include<stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT  i2c0
#define SDA_PIN 4
#define SCL_PIN 5
#define SLAVE_ADD 0x17

int main(){
stdio_init_all();

printf("I2C SLAVE start \n");


//INIt I2c at 100 mkz
//i2c_init(I2C_PORT , 100*1000);
i2c_init(I2C_PORT, 1000 * 1000);
//CONNECT GP4 AND GP5 TO I2C  hardware
gpio_set_function(SDA_PIN , GPIO_FUNC_I2C);
gpio_set_function(SCL_PIN , GPIO_FUNC_I2C);


//I2C needs pull_up register. internal pull-up is okey for first lab.
gpio_pull_up(SDA_PIN);
gpio_pull_up(SCL_PIN);
uint8_t tx_data = 0xA5 ; 
while (1){
 int ret = i2c_write_blocking(I2C_PORT , SLAVE_ADD, &tx_data ,
                            1 
                            ,false); 
   
  if(ret == 1 ){
  
 printf("master send 0x%02x to address 0x%02x:ACK received \n " , tx_data , SLAVE_ADD);
  
} else {
     printf("master failed. No ACK. RWT = %d\n", ret);
 }

}
}