#include "common.h"
#include <avr/io.h>
#include <util/delay.h>

#define DIO PB6
#define CLK PB7
#define LOW 0
#define HIGH 1
#define IN 0
#define OUT 1

#define CMD_AUTO_ADDR 0x40
#define NUM_DIGITS 4
#define START_ADDR 0xC0
#define DISPLAY_ON 0x88
#define COLON_FLAG 0x80

void bitDelay(void)
{
	_delay_us(100);
}

void setClk(int state)
{
	if (state == LOW)
		PORTB &= ~(1 << CLK);
	if (state == HIGH)
		PORTB |= (1 << CLK);
}

void setDio(int state)
{
	if (state == LOW)
		PORTB &= ~(1 << DIO);
	if (state == HIGH)
		PORTB |= (1 << DIO);
}

void start(void)
{
	/*
	 * When CLK is high, and DIO goes from high to low, input begins
	 */
	setClk(HIGH);
	setDio(HIGH);
	bitDelay();
	setDio(LOW);
	bitDelay();
	setClk(LOW);
	bitDelay();
}

void stop(void)
{
	/*
	* When CLK is high, and DIO goes from low to high, input ends
	*/
	setClk(LOW);
	setDio(LOW);
	bitDelay();
	setClk(HIGH);
	bitDelay();
	setDio(HIGH);
	bitDelay();
}

void write(char data)
{
	/*
	*Send each bit of data
	*/
	for (int i = 0; i < 8; i++) {
		//transfer data when clock is low, from low bit to high bit
		setClk(LOW);
		setDio(data & 0x01);
		data >>= 1;
		bitDelay();
		setClk(HIGH);
		bitDelay();
	}
	/*
	* End of 8th clock cycle is the start of ACK from TM1637
	*/
	setClk(LOW);
	bitDelay();
	setClk(HIGH);
	bitDelay();
	setClk(LOW);
}

const char displayDigits[16] = {
	//
	//  AAAA
	// F    B
	// F    B
	// F    B
	// F    B
	//  GGGG
	// E    C
	// E    C
	// E    C
	// E    C
	//  DDDD  H

	0x3F,
	0x06,
	0x5B,
	0x4F,
	0x66,
	0x6D,
	0x7D,
	0x07,
	0x7F,
	0x6F,
	0x77,
	0x7C,
	0x39,
	0x5E,
	0x79,
	0x71
};

char inline convertChar(char ch) {
	return displayDigits[ch];
}

void fourDigit_display(char* digits, int light, bool colon) {
	start();
	write(CMD_AUTO_ADDR);
	stop();
	start();
	write(START_ADDR);
	for (int i = 0; i < NUM_DIGITS; i++) {
		char digit_to_write = digits[i];
		write(convertChar(digits[i]) | (COLON_FLAG * colon));
	}
	stop();
	start();
	//This sets it to the brightest
	write(DISPLAY_ON | (light & 0x07));
	stop();
}

void dbg_init() {
	DDRB = 1 << DDB6 | 1 << DDB7;
}

void dbg_print(unsigned int number) {
	char digits[4];
	digits[3] = number % 16;
	number /= 16;
	digits[2] = number % 16;
	number /= 16;
	digits[1] = number % 16;
	number /= 16;
	digits[0] = number % 16;
	for (unsigned int i = 0; i < 4; ++i) {
		digits[i] %= 0x10;
		number /= 0x10;
	}
	fourDigit_display(digits, 0xFF, false);
}