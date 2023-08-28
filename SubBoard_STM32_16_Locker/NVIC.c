
#include "NVIC.h"

void NVIC_EnableInterrupt(IRQn_Type IRQ_number)
{
  NVIC->ISER[IRQ_number/32] |= (1u<<(IRQ_number%32));
}

void NVIC_DisableInterrupt(IRQn_Type IRQ_number)
{
  NVIC->ICER[IRQ_number/32] |= (1u<<(IRQ_number%32));
}

void NVIC_ClearPendingFlag(IRQn_Type IRQ_number)
{
  NVIC->ICPR[IRQ_number/32] |= (1u<<(IRQ_number%32));
}

void NVIC_SetPriority(IRQn_Type IRQ_number, unsigned char priority)
{
  NVIC->IPR[IRQ_number/4] |= ((unsigned int)priority << (8 *(IRQ_number % 4) + 4));
}

void NVIC_SystemReset(void)
{
  SCB->AIRCR  = (NVIC_AIRCR_VECTKEY | (SCB->AIRCR & (0x700)) | (1<<NVIC_SYSRESETREQ)); /* Keep priority group unchanged */
                                                                                   /* Ensure completion of memory access */              
  while(1);                                                                                /* wait until reset */
}


