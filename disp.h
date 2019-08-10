#pragma once

#define DISP_WIDTH 296ULL
#define DISP_HEIGHT 128ULL
#define DISP_BIG_FONT_WIDTH 48ULL
#define DISP_BIG_FONT_HEIGHT 64ULL
#define DISP_SMALL_FONT_WIDTH 16ULL
#define DISP_SMALL_FONT_HEIGHT 16ULL

void disp_init(void);
void disp_setMemory(const uint8_t data);
void disp_writeMemory(const uint8_t image[][DISP_WIDTH]);
void disp_displayFrame();
void disp_writeBigTime(TIME *time, bool underscored[10]);