//SA firmware with ACC-8700-I2C

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include <string.h>
#include <stdio.h>
#include "can2040/src/can2040.h"
#include "RP2040.h"
#include <math.h>

//define pins used for i2c comm with acc800
#define SDA 0
#define SCL 1

//define acc8700 parameters
#define ACC_VLSB 0.000488
#define MAG_VLSB 0.0001

//define GPIO used to comm with CAN transciever
#define CAN_RX 4
#define CAN_TX 5

// define CAN ids
#define CAN_ACC_ID (0x06)
#define CAN_MAG_ID (0x07)
#define CAN_MSG_DELAY_MS (25)

//define ADC paramters
#define ADC_VREF 3.3
#define ADC_RANGE 4096
#define ADC_NUM_L_POS 0
#define ADC_NUM_R_POS 1

//define pins for suspension position
#define SUS_POS_L 26
#define SUS_POS_R 27

//the address of the ACC8700 accel
static int addr = 0x1E; 

// Define the number of axis for each sensor
// Seems pretty obvios, but it's better to be verbose and have it defined somewhere.
// This way it's easier to read
#define SENSOR_AXIS_N (3)

//reset function of acc8700 
static int acc8700_reset() {

    // write 0000 0000 = 0x00 to accelerometer control register 1 to place FXOS8700CQ
    //into standby
    // [7-1] = 0000 000
    // [0]: active=0
    uint8_t buf[] = {0x2A, 0x00};
    if (i2c_write_blocking(i2c0, addr, buf, 2, true) == PICO_ERROR_GENERIC) {
        return -1;
    }

    // write 0001 1111 = 0x1F to magnetometer control register 1
    // [7]: m_acal=0: auto calibration disabled
    // [6]: m_rst=0: no one-shot magnetic reset
    // [5]: m_ost=0: no one-shot magnetic measurement
    // [4-2]: m_os=111=7: 8x oversampling (for 200Hz) to reduce magnetometer noise
    // [1-0]: m_hms=11=3: select hybrid mode with accel and magnetometer active
    buf[0] = 0x5B;
    buf[1] = 0x1F; 
    if(i2c_write_blocking(i2c0, addr, buf, 2, true) == PICO_ERROR_GENERIC) {
        return -1;
    }
    
    // write 0010 0000 = 0x20 to magnetometer control register 2
    // [7]: reserved
    // [6]: reserved
    // [5]: hyb_autoinc_mode=1 to map the magnetometer registers to follow the
    // accelerometer registers
    // [4]: m_maxmin_dis=0 to retain default min/max latching even though not used
    // [3]: m_maxmin_dis_ths=0
    // [2]: m_maxmin_rst=0
    // [1-0]: m_rst_cnt=00 to enable magnetic reset each cycle 
    buf[0] = 0x5C; 
    buf[1] = 0x20;
    if (i2c_write_blocking(i2c0, addr, buf, 2, true) == PICO_ERROR_GENERIC) {
        return -1;
    }

    // write 0000 0001= 0x01 to XYZ_DATA_CFG register
    // [7]: reserved
    // [6]: reserved
    // [5]: reserved
    // [4]: hpf_out=0
    // [3]: reserved
    // [2]: reserved
    // [1-0]: fs=01 for accelerometer range of +/-4g range with 0.488mg/LSB
    buf[0] = 0x0E;
    buf[1] = 0x01;
    if(i2c_write_blocking(i2c0, addr, buf, 2, true) == PICO_ERROR_GENERIC) {
        return -1;   
    }

    // write 0000 1101 = 0x0D to accelerometer control register 1
    // [7-6]: aslp_rate=00
    // [5-3]: dr=001 for 200Hz data rate (when in hybrid mode)
    // [2]: lnoise=1 for low noise modeuint8_t buff[8];        
    // [1]: f_read=0 for normal 16 bit reads
    // [0]: active=1 to take the part out of standby and enable sampling
    buf[0] = 0x2A;
    buf[1] = 0x0D;
    if(i2c_write_blocking(i2c0, addr, buf, 2, true) == PICO_ERROR_GENERIC) {
        return -1;
    }

    return 0;      
}

//read data from acc8700
static int acc8700_read(int16_t *accel, int16_t *magn) {
    
    //the read buffer
    uint8_t buffer[13];

    //register value of status byte for accelerometer
    uint8_t val = 0x00;

    //put the register for accelerometer reading on i2c data bus
    if(i2c_write_blocking(i2c0, addr, &val, 1, true) == PICO_ERROR_GENERIC) {
        return -1;
    }

    //read the data from the ACCEL registers
    if(i2c_read_blocking(i2c0, addr, buffer, 13, false) == PICO_ERROR_GENERIC) {
        return -1;
    }

    //combine read data (8 bits) into 16 bit values
    *accel = (buffer[1] << 8 | buffer[2]>> 2);
    *(accel+1) = (buffer[3] << 8 | buffer[4]>> 2);
    *(accel+2) = (buffer[5] << 8 | buffer[6]>> 2);    
    
    //combine read data (8 bits) into 16 bit values
    *magn = (buffer[7] << 8 | buffer[8]);
    *(magn+1) = (buffer[9] << 8 | buffer[10]);
    *(magn+2) = (buffer[11] << 8 | buffer[12]); 

    return 0;
}

volatile bool flag = false;

static struct can2040 cbus;
struct can2040_msg inbound;
struct can2040_msg outbound;
    
//flow to read data from the CAN bus
static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    //memcpy(&inbound, msg, sizeof(struct can2040_msg));
    //flag = true;
} 

static void PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}

void canbus_setup(void)
{
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 500e3;
    uint32_t gpio_rx = 4, gpio_tx = 5;

    // Setup canbus
    can2040_setup(&cbus, pio_num);
    can2040_callback_config(&cbus, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0_IRQn, PIOx_IRQHandler);
    NVIC_SetPriority(PIO0_IRQ_0_IRQn, 1);
    NVIC_EnableIRQ(PIO0_IRQ_0_IRQn);

    // Start canbus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

//function that formats the data into values that can be transmitted via CAN
static void can_transmit(struct can2040_msg *outbound) {
    if(can2040_check_transmit(&cbus)) {
        if(can2040_transmit(&cbus, outbound) == 0) {
            printf("Packet id [%d] was sucessfully transmitted via CAN \n", outbound->id);
        }
        else {
            printf("Packet [%d] was not transmitted \n", outbound->id);  
        }
    }
    else {
        printf("No space \n");
    }
}

static void convert_to_uint8_payload(uint8_t *result, float *payload, int payload_size) {
    for(int i = 0; i < payload_size; i++) {
        int data = floor(payload[i] * 100);
        //The & 0xff might not be needed, but better keep it for safety.
        result[i*2] = (uint8_t)((data >> 8) & 0xff);;
        result[i*2 + 1] = (uint8_t)(data & 0xff);
    }
}

static void send_acc_data(float *acc, float suspension_left) {
    struct can2040_msg msg;
    msg.id = CAN_ACC_ID;

    // Convert the data
    convert_to_uint8_payload(&(msg.data[0]), acc, SENSOR_AXIS_N);
    convert_to_uint8_payload(&(msg.data[6]), &suspension_left, 1);

    // Send data.
    // TODO: Add error codes here and treat error cases
    can_transmit(&msg);
}

static void send_mag_data(float *mag, float suspension_right) {
    struct can2040_msg msg;
    msg.id = CAN_MAG_ID;

    // Convert the data
    convert_to_uint8_payload(&(msg.data[0]), mag, SENSOR_AXIS_N);
    convert_to_uint8_payload(&(msg.data[6]), &suspension_right, 1);

    // Send data.
    // TODO: Add error codes here and treat error cases
    can_transmit(&msg);
}

int main() {
    
    //initialise the USB
    stdio_init_all();

    //initialise ADC
    adc_init(); 
    adc_gpio_init(SUS_POS_L);
    adc_gpio_init(SUS_POS_R);

    //initialise suspesnsion position buffers
    uint16_t pos_l = 0;
    uint16_t pos_r = 0;    
    
    //initialise i2c pins
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(SDA, GPIO_FUNC_I2C);
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SDA);
    gpio_pull_up(SCL);    
    bi_decl(bi_2pins_with_func(SDA, SCL, GPIO_FUNC_I2C));    

    //initialise the accelerometer
    if(acc8700_reset() < 0)
        while(1) {
            printf("acc8700 reset error");
            sleep_ms(500);
        }

    //initialise necessary data        
    int16_t acceleration[3];
    int16_t magnetometer[3]; 
    float acc[3];
    float magn[3];
    float pos_l_f;
    float pos_r_f;    

    //initialise built_in led for heartbeat
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    bool set = 1;

    //initialise comm with CAN bus
    canbus_setup();    
    
    while(1) {
        
        //read the acceleration and the magnetic field
        // TODO: Add error handling. 
        acc8700_read(acceleration, magnetometer);     

        //normalise the values        
        for (int i = 0; i < 3; i++) {
            acc[i] = acceleration[i] * ACC_VLSB / 4 * 9.81;
            magn[i] = magnetometer[i] * MAG_VLSB / 1.2;
        }
        
        //read suspension position on the left
        adc_select_input(ADC_NUM_L_POS);
        pos_l = adc_read();

        //read suspension postion on the right
        adc_select_input(ADC_NUM_R_POS);        
        pos_r = adc_read();

        //transform raw pos data into %
        //Q: Why keep position as float?
        pos_l_f = pos_l / (float)(ADC_RANGE - 1) * 100.0;
        pos_r_f = pos_r / (float)(ADC_RANGE - 1) * 100.0;
       
        //send via USB acceleration and suspension position transformed from raw to %
        //DEBUG
        printf("Acc. X = %.2f m/s^2, Y = %.2f m/s^2, Z = %.2f m/s^2\n", acc[0], acc[1], acc[2]);
        printf("Magn. X = %.2f mT, Y = %.2f mT, Z = %.2f mT\n", magn[0], magn[1], magn[2]);
        printf("Suspension position L = %.2f %%, R = %.2f %%\n", pos_l_f, pos_r_f);         

        //transmit the data via CAN
        //can_transmit(outbound, acc, magn, pos_l_f, pos_r_f);
        send_acc_data(acc, pos_l_f);
        sleep_ms(CAN_MSG_DELAY_MS);
        send_mag_data(mag, pos_r_f);   
        sleep_ms(CAN_MSG_DELAY_MS);                 
        
        //heartbeat
        gpio_put(PICO_DEFAULT_LED_PIN, set);
        set = !set;     
        
        sleep_ms(25);    
        
    }

    return 0;
}


