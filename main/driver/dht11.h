#ifndef __DHT11_H
#define __DHT11_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/timer.h"

#define LOW 0
#define HIGH 1

#define OK 0
#define ERROR -1

typedef struct {
    uint8_t (*init)(void);
    uint8_t (*read_byte)(void);
} sDHT11_operations, *psDHT11_operations;

#define DHT11_DATA_PIN 7
#define DHT11_delay_us(count) usleep(count)
#define DHT11_delay_ms(count) usleep(count * 1000)

void DHT11_register_operations(psDHT11_operations opr);

#endif