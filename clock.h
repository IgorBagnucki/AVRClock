#pragma once

#include <stdint.h>

typedef struct tagTIME {
	uint8_t seconds : 7;
	uint8_t minutes : 7;
	uint8_t hours : 6;
	uint8_t days : 6;
	uint8_t weekdays : 3;
	uint8_t century : 1;
	uint8_t months : 5;
	uint8_t years;
} TIME, *PTIME;

void clk_set_time(TIME *time);
void clk_get_time(TIME *time);
void clk_set_alarm(uint8_t);
void clk_clear_alarm(void);