#include "common.h"
#include "dbg.h"
#include <util/twi.h>

bool wait_for_twcr() {
	uint8_t counter = 0;
	while (!(TWCR & (1 << TWINT))) {
		counter += 1;
		if (counter == 0)
			return false;
	}
	return true;
}

bool twi_start(uint8_t address) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	TRY(wait_for_twcr());
	if (TWSR & 0xF8 != TW_START) {
		return false;
	}
	TWDR = address;
	TWCR = (1 << TWINT) | (1 << TWEN);
	TRY(wait_for_twcr());
	if (TWSR & 0xF8 != TW_MT_SLA_ACK && TWSR & 0xF8 != TW_MR_SLA_ACK) {
		return false;
	}
	return true;
}

bool twi_start_write() {
	return twi_start(0xD0);
}

bool twi_start_read(void) {
	return twi_start(0xD1);
}

bool twi_send_byte(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	TRY(wait_for_twcr());
	if ((TWSR & 0xF8) != TW_MT_DATA_ACK) {
		return false;
	}
	return true;
}

bool twi_read_byte(bool ack, uint8_t *byte) {
	TWCR = (1 << TWINT) | (ack << TWEA) | (1 << TWEN);
	TRY(wait_for_twcr());
	*byte = TWDR;
	return true;
}

bool twi_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	return true;
}

bool twi_read(uint8_t *dst, int addr, int len) {
	TRY(twi_start_write());
	TRY(twi_send_byte(addr));
	TRY(twi_stop());
	TRY(twi_start_read());
	for (unsigned int pos = 0; pos < len; ++pos) {
		TRY(twi_read_byte(pos != (len - 1), &dst[pos]));
	}
	TRY(twi_stop());
	return true;
}

bool twi_write(uint8_t *src, int addr, int len) {
	TRY(twi_start_write());
	TRY(twi_send_byte(addr));
	for (unsigned int pos = 0; pos < len; ++pos) {
		TRY(twi_send_byte(src[pos]));
	}
	TRY(twi_stop());
	return true;
}