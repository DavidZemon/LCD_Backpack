/* File:    LCD_Backpack.c
 *
 * Author:  David Zemon
 * Project: LCD_Backpack
 */

#include "LCD_Backpack.h"

void main (void) {
	sysInit();

	testLCD();

	while (1)
		;
}

void sysInit (void) {
	SysCtlClockSet(
			SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	lcd_open(CONTROL_PORT_ENABLER, CONTROL_PORT_BASE, RS, RW, EN, DATA_PORT_ENABLER,
			DATA_PORT_BASE, DATA_MASK, DATA_LSB);
}

void testLCD (void) {
	lcd_ioctl_puts("Hello world!!! :D");
	lcd_ioctl_move(1, 0);
	lcd_ioctl_puts("David is the coolest...");
	lcd_ioctl_move(0, 23);
	lcd_ioctl_putchar('&');
}
