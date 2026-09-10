#ifndef STM32F1XX_H
#define STM32F1XX_H

#include "stm32f103xb.h"

/* Định nghĩa bổ sung cho chế độ thuần bare-metal */
typedef enum 
{ 
  RESET = 0, 
  SET = !RESET 
} FlagStatus, ITStatus;

#endif
