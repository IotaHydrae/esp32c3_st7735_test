#ifndef _OLED_H
#define _OLED_H

#include "gpio_spi.h"

typedef enum{
    DC=4,
    CS=6
}oled_pin_typedef_t;

void OLEDInit ( void );
void OLEDClear();
void OLEDPutChar ( int page, int col, char c );
void OLEDPrint ( int page, int col, char *str );
void OLEDPutPixel ( int x, int y );
void Glib_Line ( int x1, int y1, int x2, int y2 );
void Glib_Rectangle ( int x1, int y1, int x2, int y2 );
void Delayms ( int n ); //@11.0592MHz
void DelayS ( int n );  //@11.0592MHz

#endif