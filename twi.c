#include "common.h"
#include "dbg.h"
#include <util/twi.h>

void twi_start(uint8_t address) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TWSR & 0xF8 != TW_START) {
		dbg_print((0xE0 << 8) | (0xF8 & TWSR));
		return;
	}
	TWDR = address;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TWSR & 0xF8 != TW_MT_SLA_ACK && TWSR & 0xF8 != TW_MR_SLA_ACK) {
		dbg_print((0xE1 << 8) | (TWSR & 0xF8));
		return;
	}
}

void twi_start_write() {
	twi_start(0xA2);
}

void twi_start_read(void) {
	twi_start(0xA3);
}

void twi_send_byte(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if ((TWSR & 0xF8) != TW_MT_DATA_ACK) {
		dbg_print((0xE2 << 8) | (TWSR & 0xF8));
		return;
	}
}

uint8_t twi_read_byte(bool ack) {
	TWCR = (1 << TWINT) | (ack << TWEA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

void twi_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void twi_read(uint8_t *dst, int addr, int len) {
	twi_start_write();
	twi_send_byte(addr);
	twi_stop();
	twi_start_read();
	for (unsigned int pos = 0; pos < len; ++pos) {
		dst[pos] = twi_read_byte(pos != (len - 1));
	}
	twi_stop();
}

void twi_write(uint8_t *src, int addr, int len) {
	twi_start_write();
	twi_send_byte(addr);
	//twi_stop();
	//twi_start_write();
	for (unsigned int pos = 0; pos < len; ++pos) {
		twi_send_byte(src[pos]);
	}
	twi_stop();
}