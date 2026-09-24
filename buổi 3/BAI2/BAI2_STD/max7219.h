#ifndef MAX7219_H
#define MAX7219_H

#include "stm32f10x.h"

void MAX7219_Init(void);
void MAX7219_Write(uint8_t address, uint8_t data);

#endif
