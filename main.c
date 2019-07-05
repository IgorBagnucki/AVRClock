#include "dbg.h"
#include "common.h"
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
	dbg_init();
	unsigned int i = 0;
	while (1) {
		dbg_print(i);
		_delay_ms(500);
		i+=7;
	}
}