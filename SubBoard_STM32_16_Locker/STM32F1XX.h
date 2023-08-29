
#ifndef __STM32F1XX__
#define __STM32F1XX__

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/

#include "GPIO.h"
#include "CLOCK.h"
#include "TIMER.h"
#include "UART.h"
#include "NVIC.h"
#include "I2C.h"
#include "STRING.h"
#include "PCF8575.h"
#include "MAIN_APP.h"

/*================================================================================================*/

/*==================================================================================================
*                                        DEFINES AND MACROS
==================================================================================================*/
#define SETBIT(REG,VALUE,BIT) ((REG) |= (VALUE<<BIT))
#define CLEARBIT(REG,VALUE,BIT) ((REG) &= ~(VALUE<<BIT))
#define READBIT(REG,BIT) ((REG) & (1<<BIT))
#define SETALL(REG) ((REG) |= (0xFFFFFFFF))
#define CLEARALL(REG) ((REG) &= (0x00000000))

/*================================================================================================*/

/*==================================================================================================
*                                        ENUMS
==================================================================================================*/
typedef enum 
{
	LOW,
	HIGH
}DigitalState_enum;

typedef enum
{
	Disable,
	Enable
}SetClear_enum;

typedef enum 
{
	PushPull,
	OpenDrain,
	Alternate_PushPull,
	Alternate_OpenDrain
}OutputMode_enum;

typedef enum 
{
	AnalogMode,
	FloatingInput,
	InputPullUp_PullDown
}InputMode_enum;

typedef enum 
{
	No,
	PullUp,
	PullDown
}UpDown_enum;

typedef enum 
{
	OUTPUT,
	INPUT
}PinMode_enum;

typedef enum 
{
	PORTA,
	PORTB,
	PORTC,
	PORTD,
	PORTE,
	PORTF,
	PORTG
}PORT_enum;

typedef enum
{
	PIN0,
	PIN1,
	PIN2,
	PIN3,
	PIN4,
	PIN5,
	PIN6,
	PIN7,
	PIN8,
	PIN9,
	PIN10,
	PIN11,
	PIN12,
	PIN13,
	PIN14,
	PIN15
}PortPin_enum;

/*================================================================================================*/

#endif /* __STM32F1XX__ */
