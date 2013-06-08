/* File:    lcd_4bit.c
 *
 * Author:  David Zemon
 * Project: LCD_Backpack
 */

// Includes
#include "lcd_4bit.h"
#include <driverlib/sysctl.h>

// Private Globals
static uint32_t g_lcd_controlPort;		//
static uint32_t g_lcd_rs;				// Register select pin
static uint32_t g_lcd_rw;
static uint32_t g_lcd_en;
static uint32_t g_lcd_dataPort;
static uint32_t g_lcd_dataMask;
static uint8_t g_lcd_dataShift;

/************************
 *** Public Functions ***
 ************************/
void lcd_open (const uint32_t controlPortEnable, const uint32_t controlPort,
		const uint32_t rs, const uint32_t rw, const uint32_t en,
		const uint32_t dataPortEnable, const uint32_t dataPort, const uint32_t dataMask,
		uint32_t dataLSB) {
	// Save globals
	g_lcd_controlPort = controlPort;
	g_lcd_rs = rs;
	g_lcd_rw = rw;
	g_lcd_en = en;
	g_lcd_dataPort = dataPort;
	g_lcd_dataMask = dataMask;
	while (dataLSB) {
		dataLSB >>= 1;
		++g_lcd_dataShift;
	}
	--g_lcd_dataShift;

	// Turn on peripheral clocks
	SysCtlPeripheralEnable(controlPortEnable);
	SysCtlPeripheralEnable(dataPortEnable);

	// Set all pins as output
	GPIOPinTypeGPIOOutput(g_lcd_dataPort, dataMask);
	GPIOPinTypeGPIOOutput(g_lcd_controlPort, g_lcd_rs | g_lcd_rw | g_lcd_en);

	// Initialize LCD
	GPIOPinWrite(g_lcd_controlPort, g_lcd_rs | g_lcd_rw, 0); // Ensure we're writing to the control register
	SysCtlDelay(SysCtlClockGet() / (3*(500e3))); // Ensure the LCD is fully powered on

	GPIOPinWrite(g_lcd_dataPort, g_lcd_dataMask, 0x03 << g_lcd_dataMask); // Proceed with initialization...
	SysCtlDelay(SysCtlClockGet() / (3*(100e3)));
	lcd_ioctl_blink();
	SysCtlDelay(SysCtlClockGet() / (3*(100e3)));
	lcd_ioctl_blink();
	SysCtlDelay(SysCtlClockGet() / (3*(10e3)));
	lcd_ioctl_blink();
	SysCtlDelay(SysCtlClockGet() / (3*(10e3)));

	GPIOPinWrite(g_lcd_dataPort, g_lcd_dataMask, 0x02 << g_lcd_dataMask); // Set LCD in 4-bit mode
	lcd_ioctl_blink();

	lcd_ioctl_sendCmd(0x01);	// Clear the screen
	lcd_ioctl_sendCmd(0x28);	// Set for 4-bit/2-line mode
	lcd_ioctl_sendCmd(0x10);	// Set cursor
	lcd_ioctl_sendCmd(0x0C);	// Cursor blinking, turn on LCD
	lcd_ioctl_sendCmd(0x06);	// Set cursor to move to the right, disable display shift
	lcd_ioctl_move(0, 0);
}

void lcd_ioctl_clear (void) {
	lcd_ioctl_sendCmd(1);
}

void lcd_ioctl_putchar (const char c) {
	// Select the character data register within the LCD, as opposed to control
	GPIOPinWrite(g_lcd_controlPort, g_lcd_rs, g_lcd_rs);
	GPIOPinWrite(g_lcd_controlPort, g_lcd_rw, 0);

	// Send the high order bits
	GPIOPinWrite(g_lcd_dataPort, g_lcd_dataMask, ((c & 0xf0) >> 4) << g_lcd_dataShift);
	lcd_ioctl_blink();

	// Send the low order bits
	GPIOPinWrite(g_lcd_dataPort, g_lcd_dataMask, (c & 0x0f) << g_lcd_dataShift);
	lcd_ioctl_blink();
}

void lcd_ioctl_puts (const char* s) {
	while (*s) {
		lcd_ioctl_putchar(*s);
		++s;
	}
}

void lcd_ioctl_move (const uint8_t row, const uint8_t col) {
	uint8_t addr;

	// Encode the row address (select row 0 if invalid row is input
	// NOTE: The addresses have the "move cursor" command already encoded into them,
	//       the formula is "address = 0x40 + row_code + column" where row_code is one of
	//       0x00, 0x40, 0x14, 0x54 for each of rows 0, 1, 2, and 3
	switch (row) {
		case 0:
			addr = 0x80;
			break;
		case 1:
			addr = 0xC0;
			break;
		case 2:
			addr = 0x94;
			break;
		case 3:
			addr = 0xD4;
			break;
		default:
			addr = 0x80;
	}

	// Add the column
	addr += col;

	lcd_ioctl_sendCmd(addr);
}

/*************************
 *** Private Functions ***
 *************************/
static void lcd_ioctl_sendCmd (const uint8_t cmd) {
	// Select the control register within the LCD, as opposed to character data
	GPIOPinWrite(g_lcd_controlPort, g_lcd_rs | g_lcd_rw, 0);

	// Send the high order bits
	GPIOPinWrite(g_lcd_dataPort, g_lcd_dataMask, (cmd >> 4) << g_lcd_dataShift);
	lcd_ioctl_blink();

	// Send the low order bits
	GPIOPinWrite(g_lcd_dataPort, g_lcd_dataMask, (cmd & 0x0f) << g_lcd_dataShift);
	lcd_ioctl_blink();
}

static inline void lcd_ioctl_blink (void) {
	GPIOPinWrite(g_lcd_controlPort, g_lcd_en, g_lcd_en);
	SysCtlDelay(SysCtlClockGet() / (3 * (10e3)));
	GPIOPinWrite(g_lcd_controlPort, g_lcd_en, 0);
}
