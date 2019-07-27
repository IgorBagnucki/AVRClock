#include "common.h"
#include "dbg.h"
#include "clock.h"
#include "spi.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint.h>

void init(void) {
	dbg_init();
	disp_init();
	ADCSRA = 0;
	sleep_enable();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void sleep(void) {
	sei();
	sleep_cpu();

	/*
		EIMSK |= _BV(INT0);            //enable INT0
		adcsra = ADCSRA;               //save the ADC Control and Status Register A
		ADCSRA = 0;                    //disable ADC
		cli();
		mcucr1 = MCUCR | _BV(BODS) | _BV(BODSE);  //turn off the brown-out detector
		mcucr2 = mcucr1 & ~_BV(BODSE);
		MCUCR = mcucr1;
		MCUCR = mcucr2;
		sei();                         //ensure interrupts enabled so we can wake up again
		sleep_cpu();                   //go to sleep
		sleep_disable();               //wake up here
		ADCSRA = adcsra;               //restore ADCSRA
	*/
}

int main(void) {
	TIME time = {};
	init();
	time.minutes = 0x0;
	time.seconds = 0x0;
	clk_set_time(&time);

	SPI_MasterTransmit(0x41);
	SPI_MasterTransmit(0x41);
	SPI_MasterTransmit(0x41);
	SPI_MasterTransmit(0x41);
	sleep();

	while (true) {
		clk_get_time(&time);
		dbg_print((time.minutes << 8) | time.seconds);
	}
	return 0;
}