#include <stdint.h>
#include "common.h"
#include "clock.h"
#include "twi.h"

#define TIME_ADDRESS 2

void clk_set_time(TIME *time) {
	uint8_t time_array[7];
	time_array[0] = (time->seconds	& 0x7F);
	time_array[1] = (time->minutes	& 0x7F);
	time_array[2] = (time->hours	& 0x3F);
	time_array[3] = (time->days		& 0x3F);
	time_array[4] = (time->weekdays	& 0x07);
	time_array[5] = (((time->century << 7) | time->months) & 0x9F);
	time_array[6] = (time->years	& 0xFF);
	twi_write(time_array, TIME_ADDRESS, LEN(time_array));
}

void clk_get_time(TIME *time) {
	uint8_t time_array[7];
	twi_read(time_array, TIME_ADDRESS, LEN(time_array));
	time->seconds	= time_array[0] & 0x7F;
	time->minutes	= time_array[1] & 0x7F;
	time->hours		= time_array[2] & 0x3F;
	time->days		= time_array[3] & 0x3F;
	time->weekdays	= time_array[4] & 0x07;
	time->century	= (time_array[5] & 0x80) >> 7;
	time->months	= time_array[5] & 0x1F;
	time->years		= time_array[6] & 0xFF;
}