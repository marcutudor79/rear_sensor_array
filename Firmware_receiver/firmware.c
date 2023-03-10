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


volatile bool flag = false;
static struct can2040 cbus;
struct can2040_msg inbound;

static void
can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    memcpy(&inbound, msg, sizeof(struct can2040_msg));
    flag = true;
}

static void
PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}

void
canbus_setup(void)
{
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 500000;
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

int main() {

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    gpio_init(13);
    gpio_set_dir(13, GPIO_OUT);
       
    stdio_init_all();
    canbus_setup();
    bool set = 1;

    while(1) {
        
        if (flag == true) {
            
            printf("%d \n", inbound.id);
            
            for (int i = 0; i < 8; i++) {
                printf("%d \n", inbound.data[i]);
            }
            
            gpio_put(13, flag); 
        
        flag = false; 
        }           
        
        gpio_put(PICO_DEFAULT_LED_PIN, set);
        set = !set;
              
        
    }

    
}
