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

#include "dht11.h"

/**
 * @brief Do the gpio init & dht11 init function.
 * 
 * @param none
 * 
 * @return 
 *      - OK if success
 *      - Error if failed
 */
uint8_t DHT11_init(void)
{
    int ret;
    timer_config_t *timer_config_10;

    /* 初始化定时器 */
    timer_config_10 = (timer_config_t *)malloc(sizeof(timer_config_t));
    memset(timer_config_10, 0, sizeof(timer_config_t));
    timer_config_10->alarm_en = TIMER_ALARM_DIS;
    timer_config_10->auto_reload = TIMER_AUTORELOAD_DIS;
    timer_config_10->clk_src = TIMER_SRC_CLK_XTAL;
    timer_config_10->counter_dir = TIMER_COUNT_UP;
    timer_config_10->counter_en = TIMER_PAUSE;
    timer_config_10->divider = 40;
    ret = timer_init(TIMER_GROUP_1, TIMER_0, timer_config_10);

    /* 设置DHT11总线端口 */
    gpio_reset_pin(DHT11_DATA_PIN);
    gpio_set_direction(DHT11_DATA_PIN, GPIO_MODE_OUTPUT);

    /* 主机发送开始信号 18ms */
    gpio_set_level(DHT11_DATA_PIN, LOW);
    DHT11_delay_ms(18);

    /* 主机拉高并延时等待 20-40us */
    gpio_set_level(DHT11_DATA_PIN, HIGH);
    DHT11_delay_us(40);

    gpio_set_direction(DHT11_DATA_PIN, GPIO_MODE_INPUT);

    /* DHT响应信号拉低总线 80us */
    unsigned int loopCnt = 10000;
    while (gpio_get_level(DHT11_DATA_PIN) == LOW) {
        if (loopCnt-- == 0) {
            return ERROR;
        }
    }

    /* DHT拉高总线延时准备输出 80us */
    loopCnt = 10000;
    while (gpio_get_level(DHT11_DATA_PIN) == HIGH) {
        if (loopCnt-- == 0) {
            return ERROR;
        }
    }

    return OK;
}

/**
 * @brief Need Init DHT11 first!
 *        Read 8 bit data from DHT11_DATA_PIN and returns the byte.
 *
 * @param none
 * 
 * @return 
 *      - The byte read from DHT11_DATA_PIN if success
 *      - Error if failed
 */
uint8_t DHT11_read_byte(void)
{
    uint32_t loopCnt;
    uint8_t tmp_bytes = 0x00;
    uint64_t time_end;

    /* DHT准备开始传送数据 */
    for (int i = 0; i < 8; i++) {
        /* 保险延时，视情况可以注释掉 */
        DHT11_delay_us(20);

        /* 重载计数器中的值 */
        timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 0x0);

        /* 每一bit数据都以50us低电平时隙开始 */
        loopCnt = 10000;
        while (gpio_get_level(DHT11_DATA_PIN) == LOW) {
            if (loopCnt-- == 0) {
                return ERROR;
            }
        }

        /**
         * 数据位保持的时间，启动定时器后，
         * 带高电平持续结束，暂停定时器，锁存计数器值
         */
        loopCnt = 10000;
        timer_start(TIMER_GROUP_1, TIMER_0);
        while (gpio_get_level(DHT11_DATA_PIN) == HIGH) {
            if (loopCnt-- == 0) {
                return ERROR;
            }
        }
        timer_pause(TIMER_GROUP_1, TIMER_0);

        /**
         * 在此处判断Bit为0还是1，
         * 锁存计数器值时，需要取2次变量值才能取到正常读数
         * 原因未知
         */
        timer_get_counter_value(TIMER_GROUP_1, TIMER_0, &time_end);
        int tmp = time_end;
        tmp = time_end;

        /* 根据手册说明，若高电平持续时长26~28us，该bit为0，70us则为1 */
        tmp_bytes <<= 1;
        if (tmp > 28)
            tmp_bytes |= 1;
    }

    return tmp_bytes;
}

void DHT11_register_operations(psDHT11_operations opr)
{
    opr->init = DHT11_init;
    opr->read_byte = DHT11_read_byte;
}
