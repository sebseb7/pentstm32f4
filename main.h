#ifndef MAIN_H_
#define MAIN_H_

#if STM32F == 2
#include "stm32f2xx.h"
#endif
#if STM32F == 4
#include "stm32f4xx.h"
#endif

void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);

#endif
