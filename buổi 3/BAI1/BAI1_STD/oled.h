#ifndef OLED_H
#define OLED_H

#include "stm32f10x.h"

void OLED_Init(void);
void OLED_DrawImage(const uint8_t *image);

#endif
