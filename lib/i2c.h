#ifndef I2C_H_
#define I2C_H_

void Init_I2C1(void);

void i2cWrite(uint8_t addr,uint8_t reg, uint8_t byte);
int16_t i2cRead16s(uint8_t addr,uint8_t reg);
int16_t i2cRead16sX(uint8_t addr,uint8_t reg);

void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void I2C_write(I2C_TypeDef* I2Cx, uint8_t data);
uint8_t I2C_read_ack(I2C_TypeDef* I2Cx);
uint8_t I2C_read_nack(I2C_TypeDef* I2Cx);
void I2C_stop(I2C_TypeDef* I2Cx);
#endif
