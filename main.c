#include "common.h"
#include "dbg.h"
#include "clock.h"
#include "disp.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define RIGHT_BUTTON (!(PIND & 0x20))
#define CENTER_BUTTON (!(PIND & 0x40))
#define LEFT_BUTTON (!(PIND & 0x80))

bool left_button_int = false;
bool center_button_int = false;
bool right_button_int = false;

void init(void) {
	/* set button pins for reading */
	DDRD &= 0x1F;
	/* init debugging interface */ 
	dbg_init();
	/* init epaper display */
	disp_init();
	/* disable ADC */
	ADCSRA = 0;
	/* configure sleep mode */
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	/* enable INT0 so RTC can wake up CPU from sleep */
	// set DDD2 as input
	DDRD &= ~(1 << DDD2);
	// set INT0 to occur on low level
	EICRA = 0;
	// enable INT0
	EIMSK |= (1 << INT0);
	// and enable interrupts
	sei();
}

void sleep(void) {
	sei();
	sleep_enable();
	sleep_bod_disable();
	sleep_cpu();
	sleep_disable();
	//clk_clear_alarm();
	cli();
}

uint8_t bcd_to_uint(uint8_t bcd) {
	return (bcd % 16) + ((bcd / 16) * 10);
}

uint8_t uint_to_bcd(uint8_t uint) {
	return (uint % 10) + ((uint / 10) * 16);
}

ISR(INT0_vect) {
	left_button_int = LEFT_BUTTON;
	center_button_int = CENTER_BUTTON;
	right_button_int = RIGHT_BUTTON;
}

int main(void) {
	init();

	TIME time = {};
	time.seconds = 0x00;
	time.minutes = 25;
	time.hours = 0x22;
	time.days = 0x11;
	time.months = 0x08;
	time.years = 0x19;                      
	clk_set_time(&time);

	uint8_t underscored_index = 0;

	bool underscored_tab[8][15] = {
		{false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
		{true, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
		{false, true, false, false, false, false, false, false, false, false, false, false, false, false, false},
		{false, false, false, true, false, false, false, false, false, false, false, false, false, false, false},
		{false, false, false, false, true, false, false, false, false, false, false, false, false, false, false},
		{false, false, false, false, false, true, true, false, false, false, false, false, false, false, false},
		{false, false, false, false, false, false, false, false, true, true, false, false, false, false, false},
		{false, false, false, false, false, false, false, false, false, false, false, true, true, true, true},
	};
	while (true) {
		/* get current time */
		clk_get_time(&time);
		/* set alarm to next minute */
		clk_set_alarm(uint_to_bcd(bcd_to_uint(time.minutes) + 1));
		/* write time to screen memory */
		disp_writeBigTime(&time, underscored_tab[underscored_index]);
		/* and print it */
		disp_displayFrame();
		/* go to sleep - cpu is tired */
		sleep();
		/* if center button is pressed enter time-setting mode */
		if (center_button_int) {
			underscored_index += 1;
			underscored_index %= 8;
		}
		else if (left_button_int || right_button_int) {
			int change;
			if (left_button_int) change = -1;
			else change = 1;
			uint8_t firstDigit;
			uint8_t secondDigit;
			uint8_t bothDigits;
			switch (underscored_index)
			{
			/* hour tens */
			case 1:
				firstDigit = (time.hours & 0xF0) >> 4;
				secondDigit = time.hours & 0x0F;
				firstDigit += change;
				if (firstDigit == 0xFF) firstDigit = 2;
				else if (firstDigit == 3) firstDigit = 0;
				if (firstDigit == 2 && secondDigit > 4) { secondDigit = 4; }
				time.hours = (firstDigit << 4) | secondDigit;
				break;
			/* hour ones */
			case 2:
				firstDigit = (time.hours & 0xF0) >> 4;
				secondDigit = time.hours & 0x0F;
				secondDigit += change;
				if (secondDigit == 0xFF) secondDigit = 9;
				else if ((firstDigit != 2 && secondDigit == 10) ||
					(firstDigit == 2 && secondDigit == 5)) secondDigit = 0;
				time.hours = (firstDigit << 4) | secondDigit;
				break;
			/* minute tens */
			case 3:
				firstDigit = (time.minutes & 0xF0) >> 4;
				secondDigit = time.minutes & 0x0F;
				firstDigit += change;
				if (firstDigit == 0xFF) firstDigit = 5;
				else if (firstDigit == 6) firstDigit = 0;
				time.minutes = (firstDigit << 4) | secondDigit;
				break;
			/* minute ones */
			case 4:
				firstDigit = (time.minutes & 0xF0) >> 4;
				secondDigit = time.minutes & 0x0F;
				secondDigit += change;
				if (secondDigit == 0xFF) secondDigit = 9;
				else if (secondDigit == 10) secondDigit = 0;
				time.minutes = (firstDigit << 4) | secondDigit;
				break;
			/* days */
			case 5:
				bothDigits = bcd_to_uint(time.days);
				bothDigits += change;
				if (bothDigits == 0) bothDigits = 31;
				else if (bothDigits == 32) bothDigits = 1;
				time.days = uint_to_bcd(bothDigits);
				break;
			/* months */
			case 6:
				bothDigits = bcd_to_uint(time.months);
				bothDigits += change;
				if (bothDigits == 0) bothDigits = 12;
				else if (bothDigits == 13) bothDigits = 1;
				time.months = uint_to_bcd(bothDigits);
				break;
			/* years */
			case 7:
				bothDigits = bcd_to_uint(time.years);
				bothDigits += change;
				if (bothDigits == 0xFF || bothDigits == 100) {
					bothDigits -= change;
				}
				time.years = change;
				break;
			default:
				break;
			}
			clk_set_time(&time);
		}
		else {
			/* wait for rtc to update (why?? shouldn't it be already updated?) */
			_delay_ms(500);
		}
	}
	return 0;
}
