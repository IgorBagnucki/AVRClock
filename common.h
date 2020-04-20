#pragma once

#define F_CPU 1000000UL

#define bool char
#define true 1
#define false 0

#define LEN(x) (sizeof(x)/sizeof(x[0]))
#define TRY(x) if(!(x)) return false;