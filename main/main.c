// Copyright 2021 jensenhua(writeforever@foxmail.com)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "driver/dht11.h"

#define TIMG1_T0CONFIG_REG (*(volatile unsigned int *)0x60020000)
#define TIMG1_T0LO_REG (*(volatile unsigned int *)(0x60020000 + 0x0004))
#define TIMG1_T0HI_REG (*(volatile unsigned int *)(0x60020000 + 0x0008))
#define TIMG1_T0UPDATE_REG (*(volatile unsigned int *)(0x60020000 + 0x000c))
#define TIMG1_T0LOADLO_REG (*(volatile unsigned int *)(0x60020000 + 0x0018))
#define TIMG1_T0LOADHI_REG (*(volatile unsigned int *)(0x60020000 + 0x001c))
#define TIMG1_T0LOAD_REG (*(volatile unsigned int *)(0x60020000 + 0x0020))

static psDHT11_operations dht11_opr;
static     dht11_handle_t dht11;

#define set_gpio(pin,mode)

void System_Init(void)
{
    int result;

    gpio_reset_pin(3);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(3, GPIO_MODE_OUTPUT);

    dht11_opr = (psDHT11_operations)malloc(sizeof(sDHT11_operations));
    DHT11_register_operations(dht11_opr);

    result = dht11_opr->init(&dht11);

    if (result < 0) {
        printf("dht11 init error!\n");
        return;
    }
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
    // int ret;


    // uint8_t hum_h = dht11_opr->read_byte();
    // uint8_t hum_l = dht11_opr->read_byte();
    // uint8_t tem_h = dht11_opr->read_byte();
    // uint8_t tem_l = dht11_opr->read_byte();
    // uint8_t sum = dht11_opr->read_byte();
    // printf("温度: %0.2f℃\t湿度: %0.2f%%\n",
    //        tem_h + tem_l / 100.0, hum_h + hum_l / 100.0);
    dht11_opr->decode(&dht11);
    printf("[ %0.2f ] 温度: %0.2f℃\t湿度: %0.2f%%\n",
           //    dht11.last_decode_time/1000,
           0.0,
           dht11.temperature,
           dht11.humidity);
}

void app_main(void)
{
    TimerHandle_t xTimer_Blink, XTimer_DHT11;

    System_Init();

    xTimer_Blink = xTimerCreate(
                       "blink",
                       500,
                       pdTRUE,
                       (void *)0,
                       blink_cb);
    xTimerStart(xTimer_Blink, 0);

    XTimer_DHT11 = xTimerCreate(
                       "dht11",
                       200,
                       pdTRUE,
                       (void *)0,
                       dht11_cb);
    xTimerStart(XTimer_DHT11, 0);
}
