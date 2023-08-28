
#ifndef _I2C_Register_
#define _I2C_Register_

typedef struct 
{
  volatile unsigned int CR1;
  volatile unsigned int CR2;
  volatile unsigned int OAR1;
  volatile unsigned int OAR2;
  volatile unsigned int DR;
  volatile unsigned int SR1;
  volatile unsigned int SR2;
  volatile unsigned int CCR;
  volatile unsigned int TRISE;
}I2C_typedef;

#define I2C1_BASE_ADDRESS 	((unsigned int)0x40005400u)
#define I2C2_BASE_ADDRESS 	((unsigned int)0x40005800u)

#define I2C1                ((I2C_typedef*)I2C1_BASE_ADDRESS)
#define I2C2                ((I2C_typedef*)I2C2_BASE_ADDRESS)

#endif


