/* File:    LCD_Backpack.h
 *
 * Author:  David Zemon
 * Project: LCD_Backpack
 *
 * Description: This LCD backpack is a serial-to-parallel converter for communicating with
 * 		an LCD of the Hitachi HD44780 series. Two protocols are available by setting or
 * 		clearing pin PB5. When set, the backpack will run in I2C/TWI mode. When cleared,
 * 		SPI mode will be used.
 *
 * Pin-out:
 * 		Comm. selection:	PB5 (set = I2C, clear = SPI)
 *
 * 		RS:					PA5
 * 		R/W:				PA6
 * 		EN:					PA7
 *
 * 		Data LSB:			PC4
 * 		...
 * 		Data MSB:			PC7
 * 		(No 8-bit mode available... yet)
 */

#ifndef LCD_BACKPACK_H_
#define LCD_BACKPACK_H_

#include <stdint.h>
#include <stdbool.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_memmap.h>

#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>

#include "lcd_4bit.h"

#define CONTROL_PORT_ENABLER	SYSCTL_PERIPH_GPIOA
#define CONTROL_PORT_BASE		GPIO_PORTA_BASE
#define RS						GPIO_PIN_5
#define RW						GPIO_PIN_6
#define EN						GPIO_PIN_7

#define DATA_PORT_ENABLER		SYSCTL_PERIPH_GPIOC
#define DATA_PORT_BASE			GPIO_PORTC_BASE
#define DATA_MASK				(GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4)
#define DATA_LSB				GPIO_PIN_4

/**
 * \brief Initialize the system, ready for input
 */
void sysInit (void);

void testLCD (void);

#endif /* LCD_BACKPACK_H_ */
