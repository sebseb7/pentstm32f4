#include "main.h"
#include "arm_math.h" 

#include "lib/usb_serial.h"

/*
 *	boot loader: http://www.st.com/stonline/stappl/st/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/APPLICATION_NOTE/CD00167594.pdf (page 31)
 *  data sheet : http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/DATASHEET/CD00277537.pdf
 *
 *
 */

static __IO uint32_t TimingDelay;
static __IO uint32_t tick;
void Delay(__IO uint32_t nTime)
{
	TimingDelay = nTime*10;

	while(TimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
	tick++;
}

#ifdef DISCOVERY	
#warning buildForDisco
#else
#warning buildForSebsBoard
#endif


int main(void)
{
	RCC_ClocksTypeDef RCC_Clocks;


	RCC_GetClocksFreq(&RCC_Clocks);
	/* SysTick end of count event each 0.1ms */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 10000);
#ifdef DISCOVERY	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
#else
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
#endif


#ifdef DISCOVERY	
	GPIOD->MODER  |=    0x01<<(2*12) ; 
	GPIOD->MODER  |=    0x01<<(2*13) ; 
#else
	GPIOB->MODER  |=    0x01<<(2*12) ; 
	GPIOB->MODER  |=    0x01<<(2*13) ; 
#endif
	
	usb_serial_init();

	while(1)
	{
		usbprintf("sdfsdlkfsdjkl sdflksajdflkjsd sdlfkjsadlkfjsadl sldkafjsaldkfj sdlkfjsaldkjf sadlkfjsaldkfj sadlkfjasdlkjf \n",tick);
		usbprintf("%i",tick);

#ifdef DISCOVERY	
		GPIOD->ODR           |=       1<<12;
		GPIOD->ODR           &=       ~(1<<13);
		Delay(105);
		GPIOD->ODR           &=       ~(1<<12);
		GPIOD->ODR           |=       1<<13;
		Delay(150);
#else
		GPIOB->ODR           |=       1<<12;
		GPIOB->ODR           &=       ~(1<<13);
		Delay(105);
		GPIOB->ODR           &=       ~(1<<12);
		GPIOB->ODR           |=       1<<13;
		Delay(150);
#endif
	}

}
