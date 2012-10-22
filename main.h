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

static volatile uint32_t Timer1, Timer2;			/* 100Hz decrement timers */


#endif
