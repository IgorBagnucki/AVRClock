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

void set_time(void) {
	dbg_print(0xdead);
	for (;;);
}

EMPTY_INTERRUPT(INT0_vect);

int main(void) {
	init();

	TIME time = {};
	time.seconds = 0x00;
	time.minutes = 0x35;
	time.hours = 0x09;
	time.days = 0x09;
	time.months = 0x08;
	time.years = 0x19;                      
	clk_set_time(&time);

	bool underscored[15] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
	while (true) {
		/* get current time */
		clk_get_time(&time);
		/* set alarm to next minute */
		uint8_t alarm = time.minutes % 16 + (time.minutes / 16) * 10 + 1;
		alarm = (alarm / 10) * 16 + alarm % 10;
		clk_set_alarm(alarm);
		/* write time to screen memory */
		disp_writeBigTime(&time, underscored);
		/* and print it */
		disp_displayFrame();
		dbg_print((time.minutes << 8) | time.seconds);
		sleep();
		if (CENTER_BUTTON) {
			set_time();
		}
		/* wait for rtc to update (why?? shouldn't it be already updated?) */
		_delay_ms(500);
	}
	return 0;
}
