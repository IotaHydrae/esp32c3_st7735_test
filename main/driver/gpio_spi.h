#ifndef __GPIO_SPI_H
#define __GPIO_SPI_H

#include <stdint.h>

/**
 *  Pin connects
 *    SDA <---> IO2
 *    SCL <---> IO3
 *    D/CX  <---> IO4
 *    RES <---> IO5
 *    CS  <---> IO6
 *    BLK <---> IO7
 */
typedef enum {
    GPIO_SDA=2,
    GPIO_SCL,
    GPIO_DC,
    GPIO_RES,
    GPIO_CS,
    GPIO_BLK
} gpio_spi_t;

typedef enum {
    LOW,
    HIGH
}gpio_level_t;

/**
 * @brief
 *
 * @param
 *
 * @return
 */
uint8_t gpio_spi_read_byte();

/**
 * @brief
 *
 * @param
 *
 * @return
 */
void gpio_spi_send_byte(uint8_t byte);

/*inline void delay_ms(uint32_t count){
    count*=1000;
    while(count--){
        delay_us(1);
    }
}
inline void delay_us(uint32_t count){
    count*=1000;
    while (count--)
    {
        delay_ns(1);
    }
    
}*/
void delay_ms(uint32_t count);

#endif