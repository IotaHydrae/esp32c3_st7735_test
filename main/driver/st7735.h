// Copyright 2021 Zheng hua
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
#ifndef __ST7735_H
#define __ST7735_H

#include "gpio_spi.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief init st7735 controller.
 * 
 * @param
 * 
 * @return 
 *      - On success, returns 1
 *      - On fail returns -1
*/
uint8_t st7735_init(void);

uint8_t st7735_write_read_byte(uint8_t _byte);

/**
 * @brief send a byte to controller
 *
 * @param `_byte` a byte.
 *
 * @return 
 */
static uint8_t st7735_send_byte(uint8_t _byte);

/**
 * @brief 
 *
 * @param 
 *
 * @return 
 */
uint8_t st7735_send_command(uint8_t command);

/**
 * @brief 
 *
 * @param 
 *
 * @return 
 */
uint8_t st7735_send_data(uint8_t _data);

#ifdef __cplusplus
}
#endif

#endif