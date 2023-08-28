
#include "PCF8575.h"

unsigned char PCF8575_WritePin(const unsigned char PCF_Address, unsigned char pin, const unsigned char state) 
{
	unsigned char State=PCF8575_OK;

	unsigned char PCF8575_Port[2] = {0};

	if(pin>=1 && pin<=8)
	{
		if(pin!=0) pin = (pin-1);

		if (state==HIGH) PCF8575_Port[0] |= (1u << pin);
		else PCF8575_Port[0] &= ~(1u << pin);
	}
	else 
	{
		pin = (pin-9);

		if (state==HIGH) PCF8575_Port[1] |= (1u << pin);
		else PCF8575_Port[1] &= ~(1u << pin);
	}

	if( I2C1_SendMultiData(PCF_Address, PCF8575_Port, sizeof(PCF8575_Port))==I2C_OK )
	{
		State=PCF8575_OK;
	}
	else State=PCF8575_ERROR;

	return State;
}

unsigned char PCF8575_WriteALL(const unsigned char PCF_Address, const unsigned char state) 
{
	unsigned char State=PCF8575_OK;
	unsigned char PCF8575_Port[2] = {0};

	if (state==HIGH)
	{
		PCF8575_Port[0] = 0xFFu;
		PCF8575_Port[1] = 0xFFu;
	}

	if( I2C1_SendMultiData(PCF_Address, PCF8575_Port, sizeof(PCF8575_Port))==I2C_OK )
	{
		State=PCF8575_OK;
	}
	else State=PCF8575_ERROR;

	return State;
}

unsigned char PCF8575_ReadPin(unsigned char *StatePin , const unsigned char PCF_Address, unsigned char pin) 
{
	unsigned char State=PCF8575_OK;
	unsigned char DataRead[2] = {0};

	if( I2C1_ReadMultiData(PCF_Address,DataRead,sizeof(DataRead),I2C_ACK)==I2C_OK )
	{
		if(pin>=1 && pin<=8)
		{
			if(pin!=0) pin = (pin-1);
			*StatePin = ((DataRead[0] & (1u << pin)) ? 1 : 0);
		} 
		else
		{
			pin= pin-9;
			*StatePin = ((DataRead[1] & (1u << pin)) ? 1 : 0);
		}
	}
	else State=PCF8575_ERROR;

	return State;
}

/*===================================================================
											PCF8575 MAIN TEST
===================================================================*/

/*
void PCF8575_MAIN_TEST(void)
{
  InitClockHSE();
  Init_TIM2_Delay();
  I2C1_Init(I2C_FAST);

	Enable_Disable_Clock_PortC(Enable);
	SetPinOutput(PORTC,PIN13,PushPull);
	WritePin(PORTC,PIN13,HIGH);

	unsigned char Pin1=0;

	if( PCF8575_ReadPin(&Pin1,PCF8575_ADDRESS_READ,1)==PCF8575_OK )
	{
		WritePin(PORTC,PIN13,LOW);
		PCF8575_WriteALL(PCF8575_ADDRESS_WRITE,HIGH);
		Delay_TIM2_ms(3000);
		PCF8575_WriteALL(PCF8575_ADDRESS_WRITE,LOW);
		Delay_TIM2_ms(3000);
	}
	else 
	{
		while (1)
		{
			WritePin(PORTC,PIN13,LOW);
			Delay_TIM2_ms(100);
			WritePin(PORTC,PIN13,HIGH);
			Delay_TIM2_ms(100);
		}
		
	}

	unsigned char a,b,c,d,e,f,g;
	unsigned char i,j;

	PCF8575_WritePin(8,HIGH);

	while(1)
	{
		WritePin(PORTC,PIN13,LOW);
		Delay_TIM2_ms(1000);
		WritePin(PORTC,PIN13,HIGH);
		Delay_TIM2_ms(1000);

		Pin1++;
		if(Pin1>5) NVIC_SystemReset();

		a = PCF8575_ReadPin(PCF8575_ADDRESS_READ,1);
		b = PCF8575_ReadPin(PCF8575_ADDRESS_READ,2);
		c = PCF8575_ReadPin(PCF8575_ADDRESS_READ,3);
		d = PCF8575_ReadPin(PCF8575_ADDRESS_READ,8);
		e = PCF8575_ReadPin(PCF8575_ADDRESS_READ,9);
		f = PCF8575_ReadPin(PCF8575_ADDRESS_READ,10);
		g = PCF8575_ReadPin(PCF8575_ADDRESS_READ,16);
		
		
		for(i=1; i<=16; i++)
		{
			PCF8575_WritePin(PCF8575_ADDRESS_WRITE,i,HIGH);
			Delay_TIM2_ms(100);
		}
		Delay_TIM2_ms(1000);	


		PCF8575_WriteALL(HIGH);
		Delay_TIM2_ms(500);
		PCF8575_WriteALL(LOW);
		Delay_TIM2_ms(500);
	}
}
*/

/*=================================================================*/




