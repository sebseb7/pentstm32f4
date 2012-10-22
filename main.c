#include "main.h"
#include "arm_math.h" 
#include "stdarg.h"
#include "stdio.h"
#include "lib/imu.h"
#include "lib/usb_serial.h"
#include "lib/i2c.h"

#define FAT_WRITE
//#define USB_WRITE

#define FREEIMU
//#define MPU6150

#ifdef MPU6150
#include "mpu.h"
#endif


#ifdef FAT_WRITE
#include "lib/ff.h"
#endif


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
	static uint8_t  cntdiskio=0;

	/* every 10mSec */
	if ( cntdiskio++ >= 100 ) {
		if(Timer1 != 0)
		{
			Timer1--;
		}
		if(Timer2 != 0)
		{
			Timer2--;
		}
	}


	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
	tick++;
}



int main(void)
{
	RCC_ClocksTypeDef RCC_Clocks;

	RCC_GetClocksFreq(&RCC_Clocks);
	/* SysTick end of count event each 0.1ms */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 10000);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIOB->MODER  |=    0x01<<(2*12) ; 
	GPIOB->MODER  |=    0x01<<(2*13) ; 


#ifdef USB_WRITE
	usb_serial_init();

	Delay(10);
#endif

	Init_I2C1();


	Delay(100);

#ifdef FAT_WRITE
	FATFS FATFS_Obj;
	f_mount(0, &FATFS_Obj);
	Delay(100);
		
	FIL file;
	f_open(&file, "0:log.txt", FA_OPEN_ALWAYS | FA_WRITE);
	Delay(100);
	f_sync(&file);
	Delay(100);
	f_lseek(&file, file.fsize);
	Delay(100);
	uint8_t count = 206;
#endif

#ifdef MPU6150
	MPU6050_Initialize();
	int16_t accXbuf[20];
	int16_t accYbuf[20];
	int16_t accZbuf[20];
	uint8_t bufptr = 0;
#endif


#ifdef FREEIMU
	i2cWrite(60,0,(4<<2)|(3<<5));
	i2cWrite(60,1,1);
	i2cWrite(60,2,1);

	i2cWrite(208,22,(3<<3)|3); // set gyro to xxxx deg/sec / sample rate x hz
	i2cWrite(208,62,1); // set clock to x gyro
#endif

		
	float yaw;
	float pitch;
	float roll;

#ifdef USB_WRITE
#ifdef MPU6150
	sampleFreq = 99.94f;
#endif
#ifdef FREEIMU
	sampleFreq = 75.0f;
#endif
#endif
#ifdef FAT_WRITE
#ifdef MPU6150
	sampleFreq = 95.56f;
#endif
#ifdef FREEIMU
	sampleFreq = 72.53f;
#endif
#endif
	while(1)
	{

#ifdef FREEIMU
		int16_t gyroX = i2cRead16s(208,29)+143;
		int16_t gyroY = i2cRead16s(208,31)+17;
		int16_t gyroZ = i2cRead16s(208,33)-24;
		
		int16_t accX = (i2cRead16sX(128,2)>>2)+3;
		int16_t accY = (i2cRead16sX(128,4)>>2)+203;
		int16_t accZ = (i2cRead16sX(128,6)>>2)-403;
		
		int16_t magX = i2cRead16s(60,3)+3;
		int16_t magY = i2cRead16s(60,7)+112;
		int16_t magZ = i2cRead16s(60,5)-111;
		
		i2cWrite(60,2,1);

		float gyroX_f = gyroX / 875.0f;
		float gyroY_f = gyroY / 875.0f;
		float gyroZ_f = gyroZ / 875.0f;

		float accX_f = accX / 8000.0f;
		float accY_f = accY / 8000.0f;
		float accZ_f = accZ / 8000.0f;

		float magX_f = (magX) / 590.0f;
		float magY_f = (magY) / 590.0f;
		float magZ_f = (magZ) / 590.0f;

		MadgwickAHRSupdate(gyroX_f,gyroY_f,gyroZ_f,accX_f,accY_f,accZ_f,magX_f,magY_f,magZ_f);

		GetEulerAngles(q3,q2,q1,q0,&yaw,&pitch,&roll);

#ifdef USB_WRITE
		usbprintf("%li A: %i %i %i G: %i %i %i M: %i %i %i Q: %f %f %f %f E: %f %f %f\n",
			tick,
			accX,accY,accZ,
			gyroX,gyroY,gyroZ,
			magX,magY,magZ,
			q3,q2,q1,q0,
			pitch*M_1_PI ,roll*M_1_PI ,yaw*M_1_PI
		);
#endif
#ifdef FAT_WRITE
		char line[256];
		snprintf(line,255,"%li A: %i %i %i G: %i %i %i M: %i %i %i Q: %f %f %f %f E: %f %f %f\n",
			tick,
			accX,accY,accZ,
			gyroX,gyroY,gyroZ,
			magX,magY,magZ,
			q3,q2,q1,q0,
			pitch*M_1_PI ,roll*M_1_PI ,yaw*M_1_PI
		);
		uint bw=0;
		f_write(&file, line, strlen(line), &bw);	

		count++;
		if(count ==0)
		{
			int stat = f_sync(&file);
			if(stat == 0)
			{
				GPIOB->ODR           ^=       1<<12;
			}
		}
#endif
#endif		

#ifdef MPU6150
		int16_t rawValues[6] = {0,0,0,0,0,0};
		MPU6050_GetRawAccelGyro(rawValues);

		accXbuf[bufptr]=rawValues[0]-617;
		accYbuf[bufptr]=rawValues[1]+548;
		accZbuf[bufptr]=rawValues[2]+1500;

		bufptr++;
		if(bufptr==16)
		{
			bufptr=0;
		}

		int32_t accX=0;
		int32_t accY=0;
		int32_t accZ=0;

		for(uint8_t i = 0;i < 16;i++)
		{
			accX+=accXbuf[i];
			accY+=accYbuf[i];
			accZ+=accZbuf[i];
		}
		accX/=16;
		accY/=16;
		accZ/=16;

		rawValues[3]+=37;
		rawValues[4]+=16;
		rawValues[5]+=33;

		float gyroX_f = rawValues[3] / 875.0f;
		float gyroY_f = rawValues[4] / 875.0f;
		float gyroZ_f = rawValues[5] / 875.0f;

		float accX_f = accX / 8000.0f;
		float accY_f = accY / 8000.0f;
		float accZ_f = accZ / 8000.0f;
		
		MadgwickAHRSupdateIMU(gyroX_f,gyroY_f,gyroZ_f,accX_f,accY_f,accZ_f);
		GetEulerAngles(q3,q2,q1,q0,&yaw,&pitch,&roll);

#ifdef USB_WRITE
		usbprintf("%li A: %i %i %i G: %i %i %i Q: %f %f %f %f E: %f %f %f\n",
			tick,
			accX,accY,accZ,
			rawValues[3],rawValues[4],rawValues[5],
			q3,q2,q1,q0,
			pitch*M_1_PI ,roll*M_1_PI ,yaw*M_1_PI
		);
#endif
#ifdef FAT_WRITE
		char line[256];
		snprintf(line,255,"%li A: %i %i %i G: %i %i %i Q: %f %f %f %f E: %f %f %f\n",
			tick,
			(int16_t)accX,(int16_t)accY,(int16_t)accZ,
			rawValues[3],rawValues[4],rawValues[5],
			q3,q2,q1,q0,
			pitch*M_1_PI ,roll*M_1_PI ,yaw*M_1_PI
		);
		uint bw=0;
		f_write(&file, line, strlen(line), &bw);	

		count++;
		if(count ==0)
		{
			int stat = f_sync(&file);
			if(stat == 0)
			{
				GPIOB->ODR           ^=       1<<12;
			}
		}
#endif
#endif


		GPIOB->ODR           ^=       1<<13;
		Delay(8);
	}

}
