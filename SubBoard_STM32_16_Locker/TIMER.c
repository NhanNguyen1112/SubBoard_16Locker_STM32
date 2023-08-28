
#include "TIMER.h"

/*======================================================================================================*/
/*                              TIMER BASE                                                           */
/*======================================================================================================*/
unsigned int Tick_1us=0;
static unsigned int count=0;

void Main_TIM2_BASE(void)
{
  InitClockHSE();

	Enable_Disable_Clock_PortC(Enable);
	SetPinOutput(PORTC,PIN13,PushPull);
	WritePin(PORTC,PIN13,HIGH);

	Init_TIM2_Delay();

	while (1)
	{
    TogglePin(PORTC,PIN13);
    Delay_TIM2_ms(1000);
	}
}

void Init_TIM2_Delay(void)
{
  unsigned int CR1=0;

  TIM2_EnableClock(Enable);

  TIM2->PSC = 0u; /* Prescaler = (0-1)+1 = 0 -> 72mhz/0 = 72mhz */

  /* SYSCLK=72Mhz -> ARR=1us -> 72.000.000*0,000001= 72 tick */
  TIM2->ARR = 72u;

  TIM2->CNT = 0; /* Reset Tick_1user */

  TIM2->DIER |= (1<<0);  /* UIE: Update interrupt enable */
  TIM2->SR &= ~(1u<<0);  /* UIF: Clear UIF update interrupt flag */

  NVIC_ClearPendingFlag(TIM2_IRQn); /* Clear Pending */
  NVIC_EnableInterrupt(TIM2_IRQn); 	/* Enable interupt */

  CR1 &= ~(1u<<4); 	/* DIR set Up Tick_1user */
  CR1 |= (1u<<0); 	/* enable Tick_1user */
  TIM2->CR1 = CR1;  

  TIM2->EGR = 0x01u; /* UG Re-initialize the Tick_1user and generates an update of the registers */
}

void Delay_TIM2_ms(const unsigned int MiliSeconds)
{
  unsigned int MicroSeconds = MiliSeconds*1000;
  while(count<=MicroSeconds);
  count=0;
}

void Delay_TIM2_us(const unsigned int MicroSeconds)
{
  while(count<=(MicroSeconds-1));
  count=0;
}

void TIM2_IRQHandler(void)
{
  TIM2->SR &= ~(1u<<0);  /* UIF: Clear UIF update interrupt flag */
	Tick_1us+=1;
  count+=1;
}
/*======================================================================================================*/


