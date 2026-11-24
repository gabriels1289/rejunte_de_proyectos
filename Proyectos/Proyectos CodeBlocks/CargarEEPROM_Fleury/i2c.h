/*
* i2c.h
*
* Created: 12/08/2014 10:15:08
*  Author: gabrielBinsur
*/


#ifndef I2C_H_
#define I2C_H_

#define MAX_TRIES 50
#define EEPROM_ID    0xA0        // I2C 24FC512 EEPROM Device Identifier
#define EEPROM_ADDR  0x00        // I2C 24FC512 EEPROM Device Address
#define I2C_START 0
#define I2C_DATA  1
#define I2C_STOP  2

unsigned char i2c_transmit(unsigned char type);

int i2c_writebyte(unsigned int i2c_address, unsigned int dev_id, unsigned int dev_addr, unsigned char data);

int i2c_readbyte(unsigned int i2c_address, unsigned int dev_id, unsigned int dev_addr, unsigned char *data);

void Init_TWI(void);



#endif /* I2C_H_ */
