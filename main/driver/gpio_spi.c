#include "gpio_spi.h"

/* 使用GPIO模拟SPI */

/* 复位 */
static void SPI_GPIO_Init ( void )
{
	/* init pin here */
	gpio_reset_pin(SDA);
	gpio_reset_pin(SCK);
	gpio_reset_pin(RES);

    /* Set the GPIO as a push/pull output */
	gpio_set_direction(SDA, GPIO_MODE_OUTPUT);
	gpio_set_direction(SCK, GPIO_MODE_OUTPUT);
	gpio_set_direction(RES, GPIO_MODE_OUTPUT);
}

/* 设置SPI CLK引脚 */
static void SPI_Set_CLK ( char val )
{
	if ( val )
		gpio_set_level(SCK, 1);
	else
		gpio_set_level(SCK, 0);
}

/* 设置SPI DO引脚 */
static void SPI_Set_DO ( char val )
{
	if ( val )
		gpio_set_level(SDA, 1);

	else
		gpio_set_level(SDA, 0);
}

/* SPI按字节发送 */
void SPISendByte ( unsigned char val )
{
	int i;

	for ( i = 0; i < 8; i++ ) {
		SPI_Set_CLK ( 0 );
		SPI_Set_DO ( ( val & 0x80 ) >> 7 );
		// _nop_();
		val <<= 1;
		SPI_Set_CLK ( 1 );
		// _nop_();
		SPI_Set_CLK ( 0 );
	}
}

/* 初始化引脚 */
void SPIInit ( void )
{
	// SPI_GPIO_Init();
		
	gpio_set_level(RES, 0);
	// _nop_();
	// _nop_();
	// _nop_();
	gpio_set_level(RES, 1);
}
