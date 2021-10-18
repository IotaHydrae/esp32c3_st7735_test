#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "driver/dht11.h"
#include "driver/timer.h"

#define TIMG1_T0CONFIG_REG (*(volatile unsigned int *)0x60020000)
#define TIMG1_T0LO_REG     (*(volatile unsigned int *)(0x60020000+0x0004))
#define TIMG1_T0HI_REG     (*(volatile unsigned int *)(0x60020000+0x0008))
#define TIMG1_T0UPDATE_REG (*(volatile unsigned int *)(0x60020000+0x000c))
#define TIMG1_T0LOADLO_REG (*(volatile unsigned int *)(0x60020000+0x0018))
#define TIMG1_T0LOADHI_REG (*(volatile unsigned int *)(0x60020000+0x001c))
#define TIMG1_T0LOAD_REG   (*(volatile unsigned int *)(0x60020000+0x0020))

void set_gpio(int pin)
{
    gpio_reset_pin(pin);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

void blink_cb(xTimerHandle xTimer)
{
    /* Blink off (output low) */
    // printf("Turning on the LED\n");
    gpio_set_level(3, 1);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    /* Blink on (output high) */
    // printf("Turning off the LED\n");
    gpio_set_level(3, 0);
}

void dht11_cb(xTimerHandle xTimer)
{
    psDHT11_operations dht11_opr;
    dht11_opr = (psDHT11_operations)malloc(sizeof(sDHT11_operations));
    DHT11_register_operations(dht11_opr);
    dht11_opr->init();
    dht11_opr->read();
}

void app_main(void)
{

    int ret;
    TimerHandle_t xTimer_Blink, XTimer_DHT11;
    // sDHT11_operations my_dht11_opr;
    // DHT11_register_operations(&my_dht11_opr);
    xTimer_Blink = xTimerCreate(
                       "blink",
                       500,
                       pdTRUE,
                       (void *)0,
                       blink_cb
                   );

    set_gpio(3);
    xTimerStart(xTimer_Blink, 0);

    // XTimer_DHT11 = xTimerCreate(
    //                    "dht11",
    //                    200,
    //                    pdTRUE,
    //                    (void *)0,
    //                    dht11_cb
    //                );
    // xTimerStart(XTimer_DHT11, 0);

    timer_config_t config = {
        .alarm_en = TIMER_ALARM_DIS,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .clk_src=TIMER_SRC_CLK_XTAL,
        .counter_dir=TIMER_COUNT_UP,
        .counter_en=TIMER_PAUSE,
        .divider=40,
        .intr_type=TIMER_INTR_NONE,
    };

    timer_init(TIMER_GROUP_1, TIMER_0, &config);
    printf("TIMG1_T0CONFIG_REG: 0x%x\n", TIMG1_T0CONFIG_REG);
    TIMG1_T0LOADLO_REG=0;
    TIMG1_T0LOADHI_REG=0;
    TIMG1_T0LOAD_REG=1;

    TIMG1_T0CONFIG_REG |= (1<<31);

    uint64_t count=0;
    while(1) {
        usleep(1000*1000);
     
        printf("0x%x\n", TIMG1_T0CONFIG_REG);
        TIMG1_T0UPDATE_REG=0;
        timer_get_counter_value(1,0,&count);
        printf("count : %ld\n", count);
    }

    // timer_deinit(0,0);
}
