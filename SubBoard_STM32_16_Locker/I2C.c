
#include "I2C.h"

/* PB6:SCL - PB7:SDA */
static void I2C1_SetPin(void)
{
  Enable_Disable_Clock_PortB(Enable);
	Clock_AFIO(Enable);
  SetPinOutput(PORTB,PIN6,Alternate_OpenDrain);
  SetPinOutput(PORTB,PIN7,Alternate_OpenDrain);

  AFIO->AFIO_MAPR &= ~(1u<<1); /* No remap */
}

void I2C1_Init(I2C_Mode_Type mode)
{
  unsigned int CR1=0; 
	unsigned int CR2=0;

  I2C1_SetPin();
  I2C1_EnableClock(Enable);

  CR1 |= (1u<<15); /* Reset I2C */
  I2C1->CR1 = CR1;
  CR1=0;
  I2C1->CR1 = CR1;

  CR2 |= ((PCLK1_FREQUENCY / 1000000)<<0); /* Set prequency APB1=36Mhz (MAX) */
  I2C1->CR2 = CR2; 

  /* APB1=36Mhz -> T = 1/36 = 27.777ns
  *  CCR= (1000ns+4000ns)/27.777ns = 180 = 0xB4
  *  Hoac tra bang 41 trang 70 trong Datasheet
  */
  if (mode == I2C_STANDARD) 
	{
		I2C1->CCR &= ~(1u<<15);														/* 1: Fm mode I2C (Fast mode - 400kHz) */
		I2C1->CCR = (PCLK1_FREQUENCY / (100000 * 2));			/* CCR[11:0]: Clock control register in Fm/Sm mode (Master mode) 100kHz */
		I2C1->TRISE = ((PCLK1_FREQUENCY / 1000000) + 1);	/* TRISE[5:0]: Maximum rise time in Fm/Sm mode (Master mode) */
	} 
	else
	{
		I2C1->CCR |= (1u<<15);										/* 1: Fm mode I2C (Fast mode - 400kHz) */
		I2C1->CCR = (PCLK1_FREQUENCY / (400000 * 3));					/* CCR[11:0]: Clock control register in Fm/Sm mode (Master mode) 400kHz */
		I2C1->TRISE = (((PCLK1_FREQUENCY / 1000000 * 300) / 1000) + 1);	/* TRISE[5:0]: Maximum rise time in Fm/Sm mode (Master mode) */
	}

  CR1 |= (1u<<0); /* Enable I2C */
  I2C1->CR1 = CR1;

}

static unsigned char I2C1_Start(void)
{
	unsigned char State=I2C_OK;
	unsigned int TimerTick=0;

  I2C1->CR1 |= (1u<<8);  /* Start */

  TimerTick=Tick_1us;
	while ( !(I2C1->SR1 & (1u<<0))) /* wait generate a start condition */
	{
		if( (unsigned int)(Tick_1us - TimerTick) >= 1000 ) /* 1ms */
		{
			State=I2C_TIMEOUT;
			break;
		}
	}

	return State;
}

static unsigned char I2C1_Stop(void)
{
	unsigned char State=I2C_OK;
	unsigned int TimerTick=0;

  I2C1->CR1 |= (1u<<9); 				/* Stop I2C */

	TimerTick=Tick_1us;
	while((I2C1->SR1 & (1u<<4)))	/* 1: Stop condition detected */
	{
		if( (unsigned int)(Tick_1us - TimerTick) >= 1000 ) /* 1ms */
		{
			State=I2C_TIMEOUT;
			break;
		}
	}

	return State;
}

static unsigned char I2C1_SendAdress(unsigned int Address, I2C_Direction_Type direction)
{
	unsigned char State=I2C_OK;
	unsigned int TimerTick=0;
	
	if (direction == I2C_TRANSMITTER) 
	{
		Address &= ~(1u << 0);						/* Reset the address bit0 for write */
		I2C1->DR = Address;
	} 
	else /* Receiver */
	{
		Address |= (1u << 0);						/* Set the address bit0 for read */
		I2C1->DR = Address;
	}

	TimerTick=Tick_1us;
	while ( !((I2C1->SR1) & (1u<<1)) ) /* wait address send */		
	{
		if( (unsigned int)(Tick_1us - TimerTick) >= 1000 ) /* 1ms */
		{
			State=I2C_TIMEOUT;
			break;
		}
	}

	(void)I2C1->SR1;
	(void)I2C1->SR2;

	return State;
}

static unsigned char I2C1_Write(unsigned char Data)
{
	unsigned char State=I2C_OK;
	unsigned int TimerTick=0;

  TimerTick=Tick_1us;
	while( !(I2C1->SR1 & (1u<<7)) ) /* wait TxE set 1 -> DR empty */
	{
		if( (unsigned int)(Tick_1us - TimerTick) >= 1000 ) /* 1ms */
		{
			State=I2C_TIMEOUT;
			break;
		}
	}
	if( State==I2C_OK ) 
	{
		I2C1->DR = Data;

		TimerTick=Tick_1us;
		while( !(I2C1->SR1 & (1u<<2)) ) /* Wait byte transfer finish */
		{
			if( (unsigned int)(Tick_1us - TimerTick) >= 1000 ) /* 1ms */
			{
				State=I2C_TIMEOUT;
				break;
			}
		}
	}

	return State;
  
}

static unsigned char I2C1_Read(unsigned char *DataRead, I2C_Acknowledge_Type acknowledge)
{
	unsigned char State=I2C_OK;
	unsigned int TimerTick=0;

	if (acknowledge == I2C_ACK) 
	{
		I2C1->CR1 |= (1u<<10);					/* 1: Acknowledge returned after a byte is received (matched address or data) */
	} 
	else /* NACK */
	{
		I2C1->CR1 &= ~(1u<<10);					/* 0: No acknowledge returned */
	}

	TimerTick=Tick_1us;
	while ( !(I2C1->SR1 & (1u<<6)) ) /* RX Data not empty */
	{
		if( (unsigned int)(Tick_1us - TimerTick) >= 1000 ) /* 1ms */
		{
			State=I2C_TIMEOUT;
			break;
		}
	}

	if( State==I2C_OK )
	{
		*DataRead = ((unsigned char)I2C1->DR);
	}

	return State;
}

static unsigned char I2C1_WriteMultiData(unsigned char *Data, const unsigned char size)
{
	unsigned char State=I2C_OK;
	unsigned int TimerTick=0;
  unsigned char Count=0;

	TimerTick=Tick_1us;
	while( !(I2C1->SR1 & (1u<<7)) ) /* wait TxE set 1 -> DR empty */
	{
		if( (unsigned int)(Tick_1us - TimerTick) >= 1000 ) /* 1ms */
		{
			State=I2C_TIMEOUT;
			break;
		}
	}

  for(Count=0; Count<size; Count++)
  {
    TimerTick=Tick_1us;
		while( !(I2C1->SR1 & (1u<<7)) ) /* wait TxE set 1 -> DR empty */
		{
			if( (unsigned int)(Tick_1us - TimerTick) >= 1000 ) /* 1ms */
			{
				State=I2C_TIMEOUT;
				break;
			}
		}
		
    if( State==I2C_OK ) 
		{
			I2C1->DR = (unsigned int)*Data;
			Data++;
		}
		else 
		{
			break;
		}
    
  }
	
	if( State==I2C_OK )
	{
		TimerTick=Tick_1us;
		while( !(I2C1->SR1 & (1u<<2)) ) /* Wait byte transfer finish */
		{
			if( (unsigned int)(Tick_1us - TimerTick) >= 1000 ) /* 1ms */
			{
				State=I2C_TIMEOUT;
				break;
			}
		}
	}

	return State;
}

unsigned char I2C1_Check_Device(const unsigned char Address)
{
	unsigned char State=I2C_OK;

	if( I2C1_Start()==I2C_OK ) /* Start OK */
	{
		if( I2C1_SendAdress(Address,I2C_TRANSMITTER)==I2C_OK ) /* Send Address OK */
		{
			if( I2C1_Stop()==I2C_TIMEOUT ) /* Stop TIME OUT */
			{
				State=I2C_TIMEOUT;
			}
		}
		else State=I2C_TIMEOUT;
	}
	else State=I2C_TIMEOUT;

	return State;
}

unsigned char I2C1_SendData(const unsigned char Address, const unsigned char Data)
{
	unsigned char State=I2C_OK;

  if( I2C1_Start()==I2C_OK ) /* Start OK */
	{
		if( I2C1_SendAdress(Address,I2C_TRANSMITTER)==I2C_OK ) /* Send Address OK */
		{
			if( I2C1_Write(Data)==I2C_OK ) /* Write Data OK */
			{
				if( I2C1_Stop()==I2C_TIMEOUT ) /* Stop TIME OUT */
				{
					State=I2C_TIMEOUT;
				}
			}
			else State=I2C_TIMEOUT;
		} 
		else State=I2C_TIMEOUT;			
	}
	else State=I2C_TIMEOUT;
  
  return State;
}

unsigned char I2C1_SendMultiData(const unsigned char Address, unsigned char *Data, unsigned char size)
{
	unsigned char State=I2C_OK;

	if( I2C1_Start()==I2C_OK ) /* Start OK */
	{
		if( I2C1_SendAdress(Address,I2C_TRANSMITTER)==I2C_OK ) /* Send Address OK */
		{
			if( I2C1_WriteMultiData(Data,size)==I2C_OK ) /* Write Data OK */
			{
				if( I2C1_Stop()==I2C_TIMEOUT ) /* Stop TIME OUT */
				{
					State=I2C_TIMEOUT;
				}
			}
			else State=I2C_TIMEOUT;
		} 
		else State=I2C_TIMEOUT;		
	}
	else State=I2C_TIMEOUT;
  
  return State;
}

unsigned char I2C1_ReadData(unsigned char *DataRead, const unsigned char Address, I2C_Acknowledge_Type acknowledge)
{
	unsigned char State=I2C_OK;
	unsigned char ReadClear=0;

	if( I2C1_Start()==I2C_OK ) /* Start OK */
	{
		if( I2C1_SendAdress(Address,I2C_RECEIVER)==I2C_OK ) /* Send Address OK */
		{
			if( I2C1_Read(DataRead, acknowledge)==I2C_OK ) /* Read Data OK */
			{
				I2C1_Read(&ReadClear,I2C_NACK);
				if( I2C1_Stop()==I2C_TIMEOUT ) /* Stop TIME OUT */
				{
					State=I2C_TIMEOUT;
				}
			} else State=I2C_TIMEOUT;
		} else State=I2C_TIMEOUT;
	} else State=I2C_TIMEOUT;

	return State;
}

unsigned char I2C1_ReadMultiData(const unsigned char Address, unsigned char *DataRead, unsigned char size, I2C_Acknowledge_Type acknowledge)
{
	unsigned char State=I2C_OK;
	unsigned char ReadClear=0;
	unsigned char i=0;

	if( I2C1_Start()==I2C_OK ) /* Start OK */
	{
		if( I2C1_SendAdress(Address,I2C_RECEIVER)==I2C_OK ) /* Send Address OK */
		{
			for(i=0; i<size; i++) /* Read Multi Data */
			{
				if( I2C1_Read(DataRead, acknowledge)==I2C_OK )
				{
					DataRead++;
				} 
				else 
				{
					State=I2C_TIMEOUT;
					break;
				}
			}

			I2C1_Read(&ReadClear,I2C_NACK);
			if( I2C1_Stop()==I2C_TIMEOUT ) /* Stop TIME OUT */
			{
				State=I2C_TIMEOUT;
			}

		} else State=I2C_TIMEOUT;

	} else State=I2C_TIMEOUT;

	return State;
}

unsigned char I2C1_WriteMEM(const unsigned char AddressI2C, const unsigned char AddressMEM, unsigned char *Data, unsigned char size)
{
	unsigned char State=I2C_OK;

	if( I2C1_Start()==I2C_OK ) /* Start OK */
	{
		if( I2C1_SendAdress(AddressI2C,I2C_TRANSMITTER)==I2C_OK ) /* Send Address OK */
		{
			if( I2C1_Write(AddressMEM)==I2C_OK ) /* Write MEM Address OK */
			{
				if( I2C1_WriteMultiData(Data,size)==I2C_OK ) /* Write Data OK */
				{
					if( I2C1_Stop()==I2C_TIMEOUT ) /* Stop TIME OUT */
					{
						State=I2C_TIMEOUT;
					}
				}
				else State=I2C_TIMEOUT;

			}
			else State=I2C_TIMEOUT;

		}
		else State=I2C_TIMEOUT;

	} 
	else State=I2C_TIMEOUT;

	return State;
}

unsigned char I2C1_ReadMEM(const unsigned char AddressI2C, const unsigned char AddressMEM, \
									unsigned char *DataRead, unsigned char size, I2C_Acknowledge_Type acknowledge)
{
	unsigned char State=I2C_OK;
	unsigned char ReadClear=0;
	unsigned char i=0;

	if( I2C1_Start()==I2C_OK ) /* Start OK */
	{
		if( I2C1_SendAdress(AddressI2C,I2C_TRANSMITTER)==I2C_OK ) /* Send Address OK */
		{
			if( I2C1_Write(AddressMEM)==I2C_OK ) /* Write MEM Address OK */
			{
				if( I2C1_Stop()==I2C_TIMEOUT ) /* Stop TIME OUT */
				{
					State=I2C_TIMEOUT;
				}

			}
			else State=I2C_TIMEOUT;

		}
		else State=I2C_TIMEOUT;

	} 
	else State=I2C_TIMEOUT;

	if( I2C1_Start()==I2C_OK ) /* Start OK */
	{
		if( I2C1_SendAdress(AddressI2C,I2C_RECEIVER)==I2C_OK ) /* Send Address OK */
		{
			for(i=0; i<size; i++) /* Read Multi Data */
			{
				if( I2C1_Read(DataRead, acknowledge)==I2C_OK )
				{
					DataRead++;
				} 
				else 
				{
					State=I2C_TIMEOUT;
					break;
				}
			}

			I2C1_Read(&ReadClear,I2C_NACK);
			if( I2C1_Stop()==I2C_TIMEOUT ) /* Stop TIME OUT */
			{
				State=I2C_TIMEOUT;
			}

		}
		else State=I2C_TIMEOUT;

	} 
	else State=I2C_TIMEOUT;

	return State;
}

/*===================================================================
											I2C MAIN TEST
===================================================================*/

/*
void Main_I2C1_Test(void)
{
  InitClockHSE();
  Init_TIM2_Delay();
  I2C1_Init(I2C_STANDARD);

	unsigned char data1[10]= {'H','e','l','l','o'};

  while(1)
  {
		I2C1_SendMultiData(0x27,data1,sizeof(data1));

		// I2C_SendData(0x27,0x1);
		// Delay_TIM2_ms(100);
		// I2C_SendData(0x27,0);
		// Delay_TIM2_ms(100);
  }
}
*/

/*=================================================================*/


