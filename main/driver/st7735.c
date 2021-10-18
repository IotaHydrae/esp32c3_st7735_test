#include "st7735.h"

static uint8_t st7735_init_command_table[]= {
    0x01
};

uint8_t st7735_write_read_command(uint8_t _byte)
{
    int i;
    int tmp_byte=0x00;

    gpio_set_level(GPIO_DC, LOW);
    gpio_set_level(GPIO_CS, LOW);
    gpio_set_level(GPIO_SCL, LOW);

    for(i=0; i<8; i++) {
        gpio_set_level(GPIO_SCL, HIGH);
        gpio_set_level(GPIO_SDA, (_byte & 0x80)>>7);
        gpio_set_level(GPIO_SCL, LOW);
        _byte<<=1;
    }


    for(i=0; i<8; i++) {
        gpio_set_level(GPIO_SCL, HIGH);
        tmp_byte&=gpio_get_level(GPIO_SDA);
        gpio_set_level(GPIO_SCL, LOW);
        tmp_byte<<=1;
    }
    gpio_set_level(GPIO_CS, HIGH);

    return tmp_byte;
}

static uint8_t st7735_send_byte(uint8_t _byte)
{
    gpio_spi_read_byte(_byte);
    return 0;
}

uint8_t st7735_send_command(uint8_t command)
{
    gpio_set_level(GPIO_DC, LOW);
    return 0;
}

uint8_t st7735_send_data(uint8_t _data)
{
    gpio_set_level(GPIO_DC, HIGH);
    return 0;
}

uint8_t st7735_init(void)
{
    for(int i=0; i<sizeof(st7735_init_command_table); i++) {
        st7735_send_byte(st7735_init_command_table[i]);
    }

    return 0;
}