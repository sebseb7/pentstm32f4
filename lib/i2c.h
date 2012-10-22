#ifndef I2C_H_
#define I2C_H_

void Init_I2C1(void);

void i2cWrite(uint8_t addr,uint8_t reg, uint8_t byte);
int16_t i2cRead16s(uint8_t addr,uint8_t reg);
int16_t i2cRead16sX(uint8_t addr,uint8_t reg);

#endif
