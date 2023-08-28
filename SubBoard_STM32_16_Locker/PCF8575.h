
#ifndef __PCF8575__ 
#define __PCF8575__ 

#include "STM32F1XX.h"

typedef enum 
{
  PCF8575_OK,
  PCF8575_ERROR
}PCF8575_STATE_enum;

unsigned char PCF8575_WritePin(const unsigned char PCF_Address, unsigned char pin, const unsigned char state);
unsigned char PCF8575_WriteALL(const unsigned char PCF_Address, const unsigned char state);
unsigned char PCF8575_ReadPin(unsigned char *StatePin , const unsigned char PCF_Address, unsigned char pin);

void PCF8575_MAIN_TEST(void);

#endif /* __PCF8575__ */
