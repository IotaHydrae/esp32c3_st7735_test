#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/st7735.h"
#include "driver/gpio_spi.h"

/**
 *  Pin connects
 *    SDA <---> IO2
 *    SCL <---> IO3
 *    D/CX  <---> IO4
 *    RES <---> IO5
 *    CS  <---> IO6
 *    BLK <---> IO7
 */


static uint8_t gpio_spi_list[] = {GPIO_SDA,
                                  GPIO_SCL,
                                  GPIO_DC,
                                  GPIO_RES,
                                  GPIO_CS,
                                  GPIO_BLK
                                 };

void delay_ms(uint32_t count)
{
    vTaskDelay(count / portTICK_PERIOD_MS);
}

void set_gpio(int pin)
{
    gpio_reset_pin(pin);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

uint8_t gpio_spi_read_byte()
{
    uint8_t tmp_byte=0x00;
    for(int i=8; i>0; i--) {

    }
    return 0;
}

void gpio_spi_send_byte(uint8_t byte)
{
    gpio_set_level(GPIO_CS, LOW);
    gpio_set_level(GPIO_SCL, LOW);

    while(byte>>7!=0) {
        gpio_set_level(GPIO_SCL, HIGH);
        gpio_set_level(GPIO_SDA, byte>>7&0x1);
        gpio_set_level(GPIO_SCL, LOW);
        byte<<=1;
    }
    gpio_set_level(GPIO_CS, HIGH);
}

void blink(int pin)
{
    /* Blink off (output low) */
    printf("Turning on the LED\n");
    gpio_set_level(pin, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    /* Blink on (output high) */
    printf("Turning off the LED\n");
    gpio_set_level(pin, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void app_main(void)
{
    for (int i = 0; i < sizeof(gpio_spi_list); i++) {
        set_gpio(gpio_spi_list[i]);
    }
    /* pull up cs line. */
    gpio_set_level(GPIO_CS, HIGH);

    uint8_t ret;
    while(1) {
        ret=st7735_write_read_byte(0x04);
        printf("%d\n", ret);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
