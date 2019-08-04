#include "common.h"
#include "dbg.h"
#include "clock.h"
#include "spi.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define DISP_DD DDRC
#define DISP_DD_DC DDC0
#define DISP_DD_RST DDC1
#define DISP_DD_BUSY DDC2
#define DISP_DD_CS DDC3
#define DISP_PORT PORTC
#define DISP_DC PC0
#define DISP_RST PC1
#define DISP_BUSY PC2
#define DISP_CS PC3

#define DRIVER_OUTPUT_CONTROL                       0x01
#define BOOSTER_SOFT_START_CONTROL                  0x0C
#define GATE_SCAN_START_POSITION                    0x0F
#define DEEP_SLEEP_MODE                             0x10
#define DATA_ENTRY_MODE_SETTING                     0x11
#define SW_RESET                                    0x12
#define TEMPERATURE_SENSOR_CONTROL                  0x1A
#define MASTER_ACTIVATION                           0x20
#define DISPLAY_UPDATE_CONTROL_1                    0x21
#define DISPLAY_UPDATE_CONTROL_2                    0x22
#define WRITE_RAM                                   0x24
#define WRITE_VCOM_REGISTER                         0x2C
#define WRITE_LUT_REGISTER                          0x32
#define SET_DUMMY_LINE_PERIOD                       0x3A
#define SET_GATE_TIME                               0x3B
#define BORDER_WAVEFORM_CONTROL                     0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION        0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION        0x45
#define SET_RAM_X_ADDRESS_COUNTER                   0x4E
#define SET_RAM_Y_ADDRESS_COUNTER                   0x4F
#define TERMINATE_FRAME_READ_WRITE                  0xFF

const uint8_t lut_full_update[] =
{
	0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22,
	0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88,
	0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51,
	0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

const uint8_t lut_partial_update[] =
{
	0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void disp_send(uint8_t data) {
	DISP_PORT &= ~(1 << DISP_CS);
	SPI_MasterTransmit(data);
	DISP_PORT |= (1 << DISP_CS);
}

void disp_sendCommand(uint8_t command) {
	DISP_PORT &= ~(1 << DISP_DC);
	disp_send(command);
}

void disp_sendData(uint8_t data) {
	DISP_PORT |= (1 << DISP_DC);
	disp_send(data);
}

void disp_setLut(const uint8_t *lut) {
	disp_sendCommand(WRITE_LUT_REGISTER);
	for (unsigned int iter = 0; iter < 30; ++iter) {
		disp_sendData(lut[iter]);
	}
}

void disp_init(void) {
	SPI_MasterInit();
	/* Set ports direction */
	DISP_DD |= (1 << DISP_DD_DC) | (1 << DISP_DD_RST) | (1 << DISP_DD_CS);
	DISP_DD &= ~(1 << DISP_DD_BUSY);

	/* Reset display */
	DISP_PORT &= ~(1 << DISP_RST);
	_delay_ms(200);
	DISP_PORT |= (1 << DISP_RST);
	_delay_ms(200);
	disp_sendCommand(DRIVER_OUTPUT_CONTROL);
	disp_sendData(0x27);
	disp_sendData(0x01);
	disp_sendData(0x00);

	disp_sendCommand(BOOSTER_SOFT_START_CONTROL);
	disp_sendData(0xD7);
	disp_sendData(0xD6);
	disp_sendData(0x9D);

	disp_sendCommand(WRITE_VCOM_REGISTER);
	disp_sendData(0xA8);

	disp_sendCommand(SET_DUMMY_LINE_PERIOD);
	disp_sendData(0x1A);

	disp_sendCommand(SET_GATE_TIME);
	disp_sendData(0x08);

	disp_sendCommand(BORDER_WAVEFORM_CONTROL);
	disp_sendData(0x03);

	disp_sendCommand(DATA_ENTRY_MODE_SETTING);
	disp_sendData(0x03);

	disp_setLut(lut_full_update);
}

void disp_fillMemory(uint8_t data) {
	disp_sendCommand(SET_RAM_X_ADDRESS_START_END_POSITION);
	disp_sendData(0x00);
	disp_sendData(0x0F);

	disp_sendCommand(SET_RAM_Y_ADDRESS_START_END_POSITION);
	disp_sendData(0x00);
	disp_sendData(0x00);
	disp_sendData(0x27);
	disp_sendData(0x01);

	disp_sendCommand(SET_RAM_X_ADDRESS_COUNTER);
	disp_sendData(0x00);

	disp_sendCommand(SET_RAM_Y_ADDRESS_COUNTER);
	disp_sendData(0x00);
	disp_sendData(0x00);

	disp_sendCommand(DISPLAY_UPDATE_CONTROL_2);
	disp_sendData(0xC4);

	disp_sendCommand(WRITE_RAM);
	for (unsigned int iter = 0; iter < 4736; ++iter) {
		disp_sendData(data);
	}
}

void disp_displayFrame() {
	disp_sendCommand(MASTER_ACTIVATION);
	disp_sendCommand(TERMINATE_FRAME_READ_WRITE);
}

void init(void) {
	/* init debugging interface */ 
	dbg_init();
	/* init epaper display */
	disp_init();
	/* disable ADC */
	ADCSRA = 0;
	/* configure sleep mode */
	sleep_enable();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	/* enable INT0 so RTC can wake up CPU from sleep */
	// set DDD2 as input
	DDRD &= ~(1 << DDD2);
	// enable internal pull-down resistor
	PORTD &= ~(1 << PORTD2);
	// set INT0 to occur on rising edge
	EICRA &= (1 << ISC00) | (1 << ISC01);
	// and enable INT0
	EIMSK |= (1 << INT0);
	sei();
}

void sleep(void) {
	sleep_cpu();
	sleep_disable();

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

volatile unsigned int counter = 0;

/*ISR(INT0_vect) {
	counter++;
}*/

int main(void) {
	init();
	TIME time = {};
	time.minutes = 0x0;
	time.seconds = 0x58;
	disp_fillMemory(0xFF);
	disp_displayFrame();
	disp_fillMemory(0xFF);
	disp_displayFrame();

	while (true) {
		clk_get_time(&time);
		dbg_print((time.minutes << 8) | time.seconds);
	}
	return 0;
}