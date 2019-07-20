#pragma once

#include <stdint.h>

void twi_read(uint8_t*, int, int);
void twi_write(uint8_t*, int, int);