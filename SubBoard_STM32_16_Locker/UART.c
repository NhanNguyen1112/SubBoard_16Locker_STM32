
#include "UART.h"

void USART1_IRQHandler(void);

volatile unsigned char UART_CountRead=0;
unsigned char UART_BufferRead[UART_SIZE_BUFFER]={0};

static void Setup_GPIO_PA9TX_PA10RX(void)
{
  Enable_Disable_Clock_PortA(Enable);

  /* PA9 TX -> OUTPUT, CNF: ALT Push-pull */
  SetPinOutput(PORTA,PIN9,Alternate_PushPull);

  /* PA10 RX -> INPUT, Pull-up */
  SetPinInput(PORTA,PIN10,InputPullUp_PullDown,PullUp);

  AFIO->AFIO_MAPR &= ~(1u<<2); /* USART1 no remap */

}

void USART1_SendData(unsigned char *Data)
{
  unsigned int TimerTick=0;

  TimerTick = Tick_1us;
  while ( !(USART1->SR & (1u<<7))) /* TXE: Data is transferred to the shift register*/
  {
    if( (unsigned int)(Tick_1us - TimerTick) >= 5000 ) /* 5ms */
		{
      NVIC_SystemReset();
		}
  }

  USART1->DR = (*Data);
}

void USART1_SendMultiData(unsigned char *pTxBuffer, unsigned int Len)
{
  unsigned int i=0;
  for(i=0; i<Len; i++)
  {
    USART1_SendData(pTxBuffer);

    pTxBuffer++;
  }
}

/* PA9-TX ; PA10-RX ; Baudrate:115200 */
void USART1_Setup(void)
{
  Setup_GPIO_PA9TX_PA10RX();
  USART1_EnableClock(Enable);

  USART1->CR1 |= (1u<<3);   /* enable Transmitter */
  USART1->CR1 |= (1u<<2);   /* enable Receiver */
  USART1->CR1 &= ~(1u<<12); /* M: 1 start bit, 8 data, 1 stop bit */
  USART1->CR1 |= (1u<<5);   /* RXNEIE: RX interrupt enable */

  /* APB2 CLK= 72Mhz */
  /* Baudrate= 115200 */
  /* OVER8=0 -> sample 16 */
  /* 72.000.000/(16*115200) = 39.0625 */
  /* DIV_Mantissa = 39 */
  /* DIV_Fraction = 16*0.0625 = 1 */
  USART1->BRR |= (0x27u<<4); /* Set  DIV_Mantissa */
  USART1->BRR |= (1u<<0);   /* Set  DIV_Fraction */

  USART1->CR1 |= (1u<<13); /* Enable USART1 */

  NVIC_SetPriority(USART1_IRQn,0);    /* Set priority */
  NVIC_ClearPendingFlag(USART1_IRQn); /* interrupt clear pending USART1 */
  NVIC_EnableInterrupt(USART1_IRQn);  /* enable interrupt USART1 */

}

void USART1_IRQHandler(void)
{
  if( (USART1->SR & (1u<<5)) ) /* Data ready to be read */
  {
    UART_BufferRead[UART_CountRead] = (unsigned char)(USART1->DR & (0xFFu<<0));
    if(UART_CountRead>=(UART_SIZE_BUFFER-1)) UART_CountRead=0;
    else UART_CountRead++;
  }

  /* Test sau khi read thi gui lai Data vua read len TX */
	/*
  ReadData = (unsigned char)(USART1->DR & (0xFFu<<0));
  USART1_SendData(&ReadData,sizeof(ReadData)); 
	*/

  NVIC_ClearPendingFlag(USART1_IRQn); /* interrupt clear pending USART1 */
}

/*=============================================================================================
                                UART MAIN TEST
=============================================================================================*/

/*
void UART1_MAIN_TEST(void)
{
  InitClockHSE();
  Init_TIM2_Delay();
  USART1_Setup();

  static unsigned char SenData1[5] = {'\n','a','b','c','\n'};
  static unsigned char SenData2[5] = {'\n','1','2','3','\n'};
  
  while(1)
  {
    if(UART_CountRead != 0)
    {
      USART1_SendData("[",1);
      USART1_SendData(UART_BufferRead,sizeof(UART_BufferRead));
      USART1_SendData("]",1);
      Delay_TIM2_ms(100);

      BufferClear(UART_BufferRead, UART_SIZE_BUFFER);
      UART_CountRead=0;
    }

    // USART1_SendData(SenData1,sizeof(SenData1));
    // Delay_TIM2_ms(1000);
  }
}
*/

/*=============================================================================================*/



