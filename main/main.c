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
#define TIMG1_T0LO_REG (*(volatile unsigned int *)(0x60020000 + 0x0004))
#define TIMG1_T0HI_REG (*(volatile unsigned int *)(0x60020000 + 0x0008))
#define TIMG1_T0UPDATE_REG (*(volatile unsigned int *)(0x60020000 + 0x000c))
#define TIMG1_T0LOADLO_REG (*(volatile unsigned int *)(0x60020000 + 0x0018))
#define TIMG1_T0LOADHI_REG (*(volatile unsigned int *)(0x60020000 + 0x001c))
#define TIMG1_T0LOAD_REG (*(volatile unsigned int *)(0x60020000 + 0x0020))

void set_gpio(int pin)
{
    gpio_reset_pin(pin);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

void blink_cb(xTimerHandle xTimer)
{
    /* Blink off (output low) */
    gpio_set_level(3, 1);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gpio_set_level(3, 0);
}

void dht11_cb(xTimerHandle xTimer)
{
    int ret;

    psDHT11_operations dht11_opr;
    dht11_opr = (psDHT11_operations)malloc(sizeof(sDHT11_operations));
    DHT11_register_operations(dht11_opr);

    ret = dht11_opr->init();

    if (ret < 0) {
        printf("dht11 init error!\n");
        return;
    }

    uint8_t hum_h = dht11_opr->read_byte();
    uint8_t hum_l = dht11_opr->read_byte();
    uint8_t tem_h = dht11_opr->read_byte();
    uint8_t tem_l = dht11_opr->read_byte();
    uint8_t sum = dht11_opr->read_byte();
    printf("温度: %0.2f℃\t湿度: %0.2f%%\n",
           tem_h + tem_l / 100.0, hum_h + hum_l / 100.0);
}

void app_main(void)
{
    TimerHandle_t xTimer_Blink, XTimer_DHT11;
    
    xTimer_Blink = xTimerCreate(
                       "blink",
                       500,
                       pdTRUE,
                       (void *)0,
                       blink_cb);

    set_gpio(3);
    xTimerStart(xTimer_Blink, 0);

    XTimer_DHT11 = xTimerCreate(
                       "dht11",
                       200,
                       pdTRUE,
                       (void *)0,
                       dht11_cb);
    xTimerStart(XTimer_DHT11, 0);
}
