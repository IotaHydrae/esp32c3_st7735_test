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

#ifndef __DHT11_H
#define __DHT11_H

#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/timer.h"

typedef enum{
    DHT11_READY =0,
    DHT11_BUSY  =1,
}dht11_state_enum;

typedef enum{
    DHT11_UNLOCKED  =0,
    DHT11_LOCKED    =1,
}dht11_lock_enum;

typedef struct {

    int temperature;
    int humidity;

    int64_t timestamp;
    dht11_state_enum state;
    dht11_lock_enum lock;

}dht11_handle_t;

#define LOW 0
#define HIGH 1

#define OK 0
#define ERROR -1

#define SET_BIT(num,offset) (num|=(0x1<<offset))
#define CLR_BIT(num,offset) (num&=~(0x1<<offset))

#define __DHT11_LOCK(lock)     SET_BIT(lock, 0)
#define __DHT11_UNLOCK(lock)   CLR_BIT(lock, 0)

typedef struct {
    uint8_t (*init)(dht11_handle_t *);
    uint8_t (*decode)(dht11_handle_t *);
} sDHT11_operations,*psDHT11_operations;


#define DHT11_delay_us(count) usleep(count)
#define DHT11_delay_ms(count) usleep(count*1000)

void DHT11_register_operations(psDHT11_operations opr);



#endif