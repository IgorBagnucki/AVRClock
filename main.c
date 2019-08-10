#include "common.h"
#include "dbg.h"
#include "clock.h"
#include "disp.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint.h>

void init(void) {
	/* disable interrupts */
	cli();
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
	// enable internal pull-down resistor
	PORTD &= ~(1 << PORTD2);
	// set INT0 to occur on low level
	EICRA = 0;
	// and enable INT0
	EIMSK |= (1 << INT0);
}

void sleep(void) {
	sleep_enable();
	sleep_bod_disable();
	sei();
	sleep_cpu();
	sleep_disable();
	cli();

	/*
		mcucr1 = MCUCR | _BV(BODS) | _BV(BODSE);  //turn off the brown-out detector
		mcucr2 = mcucr1 & ~_BV(BODSE);
		MCUCR = mcucr1;
		MCUCR = mcucr2;
		sei();                         //ensure interrupts enabled so we can wake up again
		sleep_cpu();                   //go to sleep
		ADCSRA = adcsra;               //restore ADCSRA
	*/
}

ISR(INT0_vect) {
	// dummy vector to control what cpu is doing
}

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
	/*dbg_print(0xAAAA);
	sleep_cpu();
	dbg_print(0xBBBB);*/
	unsigned int prevMin = 1;
	while (true) {
		clk_get_time(&time);
		if ((time.minutes % 16) % 5 == 0 && time.minutes != prevMin) {
			prevMin = time.minutes;
			disp_writeBigTime(&time, underscored);
			disp_displayFrame();
		}
		dbg_print((time.minutes << 8) | time.seconds);
	}
	return 0;
}
