
#ifndef _USART_
#define _USART_

#include "STM32F1XX.h"
#include "UART_Register.h"

#define UART_SIZE_BUFFER 10

extern volatile unsigned char UART_CountRead;
extern unsigned char UART_BufferRead[UART_SIZE_BUFFER];

void UART1_MAIN_TEST(void);

void USART1_SendMultiData(unsigned char *pTxBuffer, unsigned int Len);
void USART1_SendData(unsigned char *Data);
void USART1_Setup(void);

#endif /* _USART_ */




