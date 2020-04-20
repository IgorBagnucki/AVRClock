#pragma once

#include <stdint.h>

bool twi_read(uint8_t*, int, int);
bool twi_write(uint8_t*, int, int);