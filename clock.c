#include <stdint.h>
#include "dbg.h"
#include "common.h"
#include "clock.h"
#include "twi.h"

#define TIME_ADDRESS 0x00

bool clk_start(void) {
	uint8_t reg;
	TRY(twi_read(&reg, TIME_ADDRESS, 1));
	reg &= 0x7F;
	TRY(twi_write(&reg, TIME_ADDRESS, 1));
	return true;
}

bool clk_stop(void) {
	uint8_t reg;
	TRY(twi_read(&reg, TIME_ADDRESS, 1));
	reg &= 0x80;
	TRY(twi_write(&reg, TIME_ADDRESS, 1));
	return true;
}

bool clk_set_time(TIME *time) {
	uint8_t time_array[7];
	time_array[0] = (time->seconds	& 0x7F);
	time_array[1] = (time->minutes	& 0x7F);
	time_array[2] = (time->hours	& 0x3F);
	time_array[3] = (time->weekdays & 0x07);
	time_array[4] = (time->days		& 0x3F);
	time_array[5] = (time->months	& 0x1F);
	time_array[6] = (time->years	& 0xFF);
	TRY(twi_write(time_array, TIME_ADDRESS, LEN(time_array)));
	return true;
}

bool clk_get_time(TIME *time) {
	uint8_t time_array[7];
	TRY(twi_read(time_array, TIME_ADDRESS, LEN(time_array)));
	time->seconds	= time_array[0] & 0x7F;
	time->minutes	= time_array[1] & 0x7F;
	time->hours		= time_array[2] & 0x3F;
	time->weekdays	= time_array[3] & 0x07;
	time->days		= time_array[4] & 0x3F;
	time->months	= time_array[5] & 0x1F;
	time->years		= time_array[6] & 0xFF;
	return true;
}
