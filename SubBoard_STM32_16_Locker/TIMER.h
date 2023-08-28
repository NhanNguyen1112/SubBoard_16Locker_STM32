
#ifndef __Timer__
#define __Timer__

#include "TIMER_Register.h"

/*======================================================================================================*/
/*                              TIMER BASE                                                           */
/*======================================================================================================*/

extern unsigned int Tick_1us;

void Init_TIM2_Delay(void);
void Delay_TIM2_ms(const unsigned int MiliSeconds);
void Delay_TIM2_us(const unsigned int MicroSeconds);
void TIM2_IRQHandler(void);
void Function_Test(void);

void Main_TIM2_BASE(void) __attribute__ ((noreturn));

/*======================================================================================================*/


#endif
