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

#define DHT11_START_TRANS_TIME 18   /* ms */
#define HOST_PULLUP_TIME       40   /* us */

#define DHT11_DATA_PIN       7
#define DHT11_BITS_PER_READ 40

/**
 * @brief Do the gpio init & dht11 init function.
 *
 * @param dht11 a handle struct to save data for module.
 *
 * @return
 *      - OK if success
 *      - Error if failed
 */
uint8_t DHT11_init(dht11_handle_t *dht11)
{
    int ret;
    timer_config_t *config_timer10;

    dht11->lock = DHT11_UNLOCKED;
    dht11->state = DHT11_BUSY;

    config_timer10 = (timer_config_t *)malloc(sizeof(timer_config_t));
    memset(config_timer10, 0, sizeof(timer_config_t));

    /* 初始化定时器 */
    config_timer10->alarm_en = TIMER_ALARM_DIS;     /* 禁用计数器报警 */
    config_timer10->auto_reload = TIMER_AUTORELOAD_DIS; /* 禁用计数器自动重载 */
    config_timer10->clk_src = TIMER_SRC_CLK_XTAL;   /* 设置时钟来源为XTAL 40MHz */
    config_timer10->counter_dir = TIMER_COUNT_UP;   /* 设置计数器值变更方式，此处为增加 */
    config_timer10->counter_en = TIMER_PAUSE;   /* 设置计数器使能 */

    /**
     * 设置计数器分频器
     * 定时器时钟=时钟源频率/分频值 = 40MHz / 40 = 1MHz
     * 周期=1/定时器时钟 1/1MHz 即 1us
     * 所以可以使用改配置进行微秒级定时。
     */
    config_timer10->divider = 40;
    ret = timer_init(TIMER_GROUP_1, TIMER_0, config_timer10);

    /* 设置DHT11总线端口 */
    gpio_reset_pin(DHT11_DATA_PIN);
    gpio_set_direction(DHT11_DATA_PIN, GPIO_MODE_INPUT_OUTPUT_OD);

    /* dht11初始化完成 */
    dht11->state = DHT11_READY;

    return ret;
}

/**
 * @brief Need Init DHT11 first!
 *        Read 8 bit data from DHT11_DATA_PIN and returns the byte.
 *
 * @param none
 *
 * @return tmp_bytes
 *      - The byte read from DHT11_DATA_PIN if success
 *      - Error if failed
 */
static uint8_t dht11_decode_byte(void)
{
    uint8_t tmp_bytes = 0x00;
    uint64_t time_end;

    /* DHT准备开始传送数据 */
    for (int i = 0; i < 8; i++) {
        /* 保险延时，视情况可以注释掉 */
        // DHT11_delay_us(20);

        /* 重载计数器中的值为0 */
        timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 0x0);

        /* 每一bit数据都以50us低电平时隙开始 */
        wait_condition_by_attempts(gpio_get_level(DHT11_DATA_PIN) == LOW, MAX_ATTEMPTS);

        /**
         * 获取数据位保持的时间
         * 启动定时器，待高电平持续结束，
         * 暂停定时器，锁存计数器值。
         */
        timer_start(TIMER_GROUP_1, TIMER_0);
        wait_condition_by_attempts(gpio_get_level(DHT11_DATA_PIN) == HIGH, MAX_ATTEMPTS);
        timer_pause(TIMER_GROUP_1, TIMER_0);

        /**
         * 写寄存器 TIMG_T0UPDATE_REG bit31任意值触发定时器锁存操作
         * 计数器 高22位 锁存 至寄存器 TIMG1_T0HI_REG中
         * 计数器 低32位 锁存 至寄存器 TIMG1_T0LO_REG中
         */
        timer_get_counter_value(TIMER_GROUP_1, TIMER_0, &time_end);

        /**
         * 在此处判断Bit为0还是1，
         * 锁存计数器值时，需要取2次变量值才能取到正常读数
         * 原因未知
         */
        int tmp_time = time_end;
        tmp_time = time_end;

        /* 根据DHT11手册说明，若高电平持续时长26~28us，该bit为0，若持续70us则为1 */
        tmp_bytes <<= 1;
        if (tmp_time > 28) tmp_bytes++;
    }

    return tmp_bytes;
}

/**
 * @brief Need Init DHT11 first!
 *        Read all 40 bit data from DHT11_DATA_PIN and returns the byte.
 *
 * @param dht11 a handle struct to save data for module.
 *
 * @return
 *      - ESP_OK  if success
 *      - Error or ESP_FAIL if failed
 */
uint8_t DHT11_decode(dht11_handle_t *dht11)
{
    uint8_t hum_int, hum_dec, temp_int, temp_dec, checksum;

    if(dht11->state != DHT11_READY) {
        return ESP_FAIL;
    }

    __DHT11_LOCK(dht11->lock);

    /* 主机发送开始信号 18ms */
    gpio_set_level(DHT11_DATA_PIN, LOW);
    DHT11_delay_ms(DHT11_START_TRANS_TIME);

    /* 主机拉高并延时等待 20-40us */
    gpio_set_level(DHT11_DATA_PIN, HIGH);
    DHT11_delay_us(HOST_PULLUP_TIME);

    /* DHT响应信号拉低总线 80us */
    wait_condition_by_attempts(gpio_get_level(DHT11_DATA_PIN) == LOW, MAX_ATTEMPTS);

    /* DHT拉高总线延时准备输出 80us */
    wait_condition_by_attempts(gpio_get_level(DHT11_DATA_PIN) == HIGH, MAX_ATTEMPTS);

    /* 读取数据，共40bit */
    hum_int  = dht11_decode_byte();     /* 湿度整数部分 */
    hum_dec  = dht11_decode_byte();     /* 湿度小数部分 */
    temp_int = dht11_decode_byte();     /* 温度整数部分 */
    temp_dec = dht11_decode_byte();     /* 温度小数部分 */
    checksum = dht11_decode_byte();     /* 校验和 */

    /* 数据转储 */
    dht11->humidity = (float)(hum_int + hum_dec / 100.0);
    dht11->temperature = (float)(temp_int + temp_dec / 100.0);
    dht11->checksum = checksum;

    __DHT11_UNLOCK(dht11->lock);

    return ESP_OK;
}

#if 0
static sDHT11_operations dht11_opr = {
    .init   = DHT11_init,
    .decode = DHT11_decode,
};
#endif

void DHT11_register_operations(psDHT11_operations opr)
{
    opr->init   = DHT11_init;
    opr->decode = DHT11_decode;
}
