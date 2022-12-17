#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "MCP2515_nb.h"

//define pins used for i2c comm with mpu6050
#define SDA  0
#define SCL  1

//define pins used for spi comm with mcp2515
#define SCK 4
#define MOSI 5
#define MISO 6
#define CS 7

//initialisation of CAN Module
//MCP2515 MCP = MCP2515();

//the address of the MPU6050 accel
static int addr = 0x68; 

//this is how the mpu6050 is reloaded
static void mpu6050_reset() {

    //resets the register values of the acceleromter
    uint8_t buf[] = {0x6B, 0x00};
    i2c_write_blocking(i2c0, addr, buf, 2, true);
    
    
    //set the acceleromter to be 4g scale 
    uint8_t buf2[] = {0x1C, 0x08};
    i2c_write_blocking(i2c0, addr, buf2, 2, true);
    
  
}

//now we read data from it
static void mpu6050_read(int16_t accel[3]) {
    
    uint8_t buffer[6];

    //register value of ACCEL_XOUT[15:8]
    uint8_t val = 0x3B;

    //put the register for accelerometer reading on i2c data bus
    i2c_write_blocking(i2c0, addr, &val, 1, true);

    //read the data from the accelerometer
    i2c_read_blocking(i2c0, addr, buffer, 6, false);

    //parse through the buffer
    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i*2] << 8 | buffer[(i*2) + 1]);
    }
}

int main() {
    
    //initialise the USB
    stdio_init_all();
    
    //initialise i2c pins
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(SDA, GPIO_FUNC_I2C);
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SDA);
    gpio_pull_up(SCL);
    
    bi_decl(bi_2pins_with_func(SDA, SCL, GPIO_FUNC_I2C));    

    /*
    //initialise spi pins
    spi_init(spi0, 1000000);
    gpio_set_function(SCK, GPIO_FUNC_SPI);
    gpio_set_function(MISO, GPIO_FUNC_SPI);
    gpio_set_function(MOSI, GPIO_FUNC_SPI);
    
    gpio_init(CS);
    gpio_set_dir(CS, GPIO_OUT);
    */

    //initialise built_in led
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    bool set = 1;
    
    //initialise the accelerometer
    mpu6050_reset();
    int16_t acceleration[3];
    /*
    //initialise the CAN module
    MCP.begin(50e3, MISO, MOSI, SCK, CS, spi0);

    //buffer to write the acc data into
    */  
    
    while(1) {
        
        //read the acceleration
        mpu6050_read(acceleration);        

        //normalise acceleration
        float acc[3];
        for (int i = 0; i < 3; i++) {
            acc[i] = acceleration[i] * 4 * 9.81 / 32768;
        }

        //send it via USB
        printf("Acc. X = %f, Y = %f, Z = %f\n", acc[0], acc[1], acc[2]);

        /*
        //send it via CAN
        CANPacket std = CANPacket();

        std.startStandard(0x80);
        std.writeData(acceleration[0]); // up to 8 data bytes
        std.writeData(acceleration[1]);
        std.writeData(acceleration[2]);        
        std.end(); 
        */

        //blink the built in led
        gpio_put(PICO_DEFAULT_LED_PIN, set);
        if (set == 1)
            set = 0;
        else
            set = 1;     

        sleep_ms(100);
        
    }

}


