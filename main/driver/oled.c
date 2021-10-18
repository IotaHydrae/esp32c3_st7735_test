#include <math.h>
#include "oled.h"
#include "oledfont.h"
#include "gpio_spi.h"

#if 0
static void OLED_Set_DC ( char val )
{
	if ( val )
		DC = 1;

	else
		DC = 0;
}

static void OLED_Set_CS ( char val )
{
	if ( val )
		CS = 1;

	else
		CS = 0;
}
#endif

void OLED_GPIO_Init(void) 
{
	/* init pin here */
	gpio_reset_pin(DC);
	gpio_reset_pin(CS);

    /* Set the GPIO as a push/pull output */
	gpio_set_direction(CS, GPIO_MODE_OUTPUT);
	gpio_set_direction(DC, GPIO_MODE_OUTPUT);
}

static void OLED_Set_DC ( char val )
{
	if ( val )
		gpio_set_level(DC, 1);

	else
		gpio_set_level(DC, 0);
}

static void OLED_Set_CS ( char val )
{
	if ( val )
		gpio_set_level(CS, 1);

	else
		gpio_set_level(CS, 0);
}

static void OLEDWriteCmd ( unsigned char cmd )
{
	OLED_Set_DC ( 0 ); /*command*/
	OLED_Set_CS ( 0 ); /* selected */

	SPISendByte ( cmd );

	OLED_Set_DC ( 1 );
	OLED_Set_CS ( 1 );
}

static void OLEDWriteDat ( unsigned char dat )
{
	OLED_Set_DC ( 1 ); /*dat*/
	OLED_Set_CS ( 0 ); /* selected */

	SPISendByte ( dat );

	OLED_Set_DC ( 1 );
	OLED_Set_CS ( 1 );
}


static void OLEDSetPos ( int page, int col )
{
	/* 设置页地址 */
	OLEDWriteCmd ( 0xB0 + page );

	/* 设置列地址 */
	OLEDWriteCmd ( 0x00 | ( col & 0x0f ) ); /* 低地址 */
	OLEDWriteCmd ( 0x10 | ( col >> 4 ) ); /* 高地址 */
}

void OLEDClear()
{
	int page, col;

	for ( page = 0; page < 8; page++ ) {
		OLEDSetPos ( page, 0 );

		for ( col = 0; col < 128; col++ )
			OLEDWriteDat ( 0 );
	}
}

/*
    A[1:0] = 00b, Horizontal Addressing Mode
    A[1:0] = 01b, Vertical Addressing Mode
    A[1:0] = 10b, Page Addressing Mode (RESET)
    A[1:0] = 11b, Invalid

*/
static void OLEDSetMemoryAddressingMode ( int mode )
{
	OLEDWriteCmd ( 0x20 );

	switch ( mode ) {
		case 0:
			OLEDWriteCmd ( 0x00 );
			break; // Horizontal Addressing Mode

		case 1:
			OLEDWriteCmd ( 0x01 );
			break; // Vertical Addressing Mode

		case 2:
			OLEDWriteCmd ( 0x02 );
			break; // Page Addressing Mode

		case 3:
			OLEDWriteCmd ( 0x03 );
			break; // Invalid

		default:
			OLEDWriteCmd ( 0x02 );
			break; // Page Addressing Mode
	}
}


void OLEDInit ( void )
{
	OLED_GPIO_Init();

	OLEDWriteCmd ( 0xae ); //--turn off oled panel

	OLEDWriteCmd ( 0x00 ); //--set low column address
	OLEDWriteCmd ( 0x10 ); //--set high column address

	OLEDWriteCmd ( 0x40 ); //--set start line address

	OLEDWriteCmd ( 0xb0 ); //--set page address

	OLEDWriteCmd ( 0x81 ); //--set contrast control register
	OLEDWriteCmd ( 0xff );

	OLEDWriteCmd ( 0xa1 ); //--set segment re-map 127 to 0   a0:0 to seg127
	OLEDWriteCmd ( 0xa6 ); //--set normal display

	OLEDWriteCmd ( 0xc9 ); //--set com(N-1)to com0  c0:com0 to com(N-1)

	OLEDWriteCmd ( 0xa8 ); //--set multiples ratio(1to64)
	OLEDWriteCmd ( 0x3f ); //--1/64 duty

	OLEDWriteCmd ( 0xd3 ); //--set display offset
	OLEDWriteCmd ( 0x00 ); //--not offset

	OLEDWriteCmd ( 0xd5 ); //--set display clock divide ratio/oscillator frequency
	OLEDWriteCmd ( 0x80 ); //--set divide ratio

	OLEDWriteCmd ( 0xd9 ); //--set pre-charge period
	OLEDWriteCmd ( 0xf1 );

	OLEDWriteCmd ( 0xda ); //--set com pins hardware configuration
	OLEDWriteCmd ( 0x12 );

	OLEDWriteCmd ( 0xdb ); //--set vcomh
	OLEDWriteCmd ( 0x40 );

	OLEDWriteCmd ( 0x8d ); //--set chare pump enable/disable
	OLEDWriteCmd ( 0x14 ); //--set(0x10) disable
	// OLEDWriteCmd(0x10);  //--set(0x10) disable
	OLEDWriteCmd ( 0xaf ); //--turn on oled panel

	OLEDClear();
	OLEDSetMemoryAddressingMode ( 2 );
}

void OLEDPutChar ( int page, int col, char c )
{
	int i = 0;
	/* 获取字模 */
	const unsigned char *dots = oled_asc2_8x16[c - ' '];

	/* 发送到OLED */
	OLEDSetPos ( page, col );

	/* 发出8字节数据 */
	for ( i = 0; i < 8; i++ )
		OLEDWriteDat ( dots[i] );

	/* 再次发送8字节数据 */
	OLEDSetPos ( page + 1, col );

	for ( i = 0; i < 8; i++ )
		OLEDWriteDat ( dots[i + 8] );
}

/*
    page: 0-7
    col : 0-127
*/
void OLEDPrint ( int page, int col, char *str )
{
	/* res = 128*64 */
	int i = 0;

	while ( str[i] ) {
		OLEDPutChar ( page, col, str[i] );
		col += 8;

		if ( col > 127 ) {
			col = 0;
			page += 2;
		}

		i++;
	}
}
