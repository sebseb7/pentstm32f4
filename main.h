#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f2xx.h"

void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);

static volatile uint32_t Timer1, Timer2;			/* 100Hz decrement timers */


#endif
