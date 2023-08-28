
#ifndef _I2C_
#define _I2C_

#include "STM32F1XX.h"
#include "I2C_Register.h"

#define PCLK1_FREQUENCY	((unsigned int)(72000000u / 2u))	/* PCLK1 frequency */

typedef enum {
	I2C_STANDARD,
	I2C_FAST
} I2C_Mode_Type;

typedef enum {
	I2C_TRANSMITTER,
	I2C_RECEIVER
} I2C_Direction_Type;

typedef enum {
	I2C_ACK,
	I2C_NACK
} I2C_Acknowledge_Type;

typedef enum {
	I2C_OK,
	I2C_TIMEOUT
} I2C_Status_Type;

void I2C1_Init(I2C_Mode_Type mode);
unsigned char I2C1_Check_Device(const unsigned char Address);

unsigned char I2C1_SendData(const unsigned char Address, const unsigned char Data);
unsigned char I2C1_SendMultiData(const unsigned char Address, unsigned char *Data, unsigned char size);

unsigned char I2C1_WriteMEM(const unsigned char AddressI2C, const unsigned char AddressMEM, unsigned char *Data, unsigned char size);
unsigned char I2C1_ReadMEM(const unsigned char AddressI2C, const unsigned char AddressMEM, \
									unsigned char *Data, unsigned char size, I2C_Acknowledge_Type acknowledge);

unsigned char I2C1_ReadData(unsigned char *DataRead, const unsigned char Address, I2C_Acknowledge_Type acknowledge);
unsigned char I2C1_ReadMultiData(const unsigned char Address, unsigned char *DataRead, unsigned char size, I2C_Acknowledge_Type acknowledge);


#endif


