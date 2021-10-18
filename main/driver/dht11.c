#include "dht11.h"

uint8_t DHT11_init(void)
{
    int ret;
    timer_config_t *timer_config_10;

    gpio_reset_pin(DHT11_DATA_PIN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(DHT11_DATA_PIN, GPIO_MODE_OUTPUT);

    timer_config_10 = (timer_config_t *)malloc(sizeof(timer_config_t));
    memset(timer_config_10, 0, sizeof(timer_config_t));

    timer_config_10->alarm_en=TIMER_ALARM_EN;
    timer_config_10->auto_reload=TIMER_AUTORELOAD_EN;
    timer_config_10->clk_src=TIMER_SRC_CLK_APB;
    timer_config_10->counter_dir=TIMER_COUNT_UP;
    timer_config_10->counter_en=TIMER_PAUSE;
    timer_config_10->divider=16;

    ret = timer_init(1, 0, timer_config_10);
    if(ret!=ESP_OK) {
        printf("init failed\n");
        return ERROR;
    }

    // timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 0);

    return OK;
}

uint8_t DHT11_read(void)
{
    uint16_t bit_count=7;
    uint8_t tmp_bytes[5];
    uint8_t byte_index=0;
    uint64_t time_end, time_start;

    gpio_set_direction(DHT11_DATA_PIN, GPIO_MODE_OUTPUT);
    /* 主机发送开始信号 18ms */
    gpio_set_level(DHT11_DATA_PIN, LOW);
    DHT11_delay_ms(18);

    /* 主机拉高并延时等待 20-40us */
    gpio_set_level(DHT11_DATA_PIN, HIGH);
    DHT11_delay_us(40);
    gpio_set_direction(DHT11_DATA_PIN, GPIO_MODE_INPUT);

    /* DHT响应信号拉低总线 80us */
    unsigned int loopCnt=10000;
    while(gpio_get_level(DHT11_DATA_PIN)==LOW) {
        if(loopCnt-- == 0) {
            return ERROR;
        }
    }

    /* DHT拉高总线延时准备输出 80us */
    loopCnt=10000;
    while(gpio_get_level(DHT11_DATA_PIN)==HIGH) {
        if(loopCnt-- == 0) {
            return ERROR;
        }
    }

    /* DHT准备开始传送数据 */
    for(int i=0; i<40; i++) {
        
        /* 每一bit数据都以50us低电平时隙开始 */
        loopCnt=1000;
        while(gpio_get_level(DHT11_DATA_PIN)==LOW) {
            if(loopCnt-- == 0) {
                return ERROR;
            }
        }

        /* 数据位保持的时间 */
        timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 0x00ull);
        timer_start(TIMER_GROUP_1, TIMER_0);
        loopCnt=1000;
        while(gpio_get_level(DHT11_DATA_PIN)==HIGH) {
            if(loopCnt-- == 0) {
                return ERROR;
            }
        }
        timer_pause(TIMER_GROUP_1, TIMER_0);

        /* 在此处判断Bit为0还是1 */
        
        timer_get_counter_value(TIMER_GROUP_1, TIMER_0, &time_end);
        
        printf("end at :  %ld\n", time_end);
    }

    return OK;
}

uint8_t DHT11_register_operations(psDHT11_operations opr)
{

    opr->init=DHT11_init;
    opr->read=DHT11_read;

    return OK;
}