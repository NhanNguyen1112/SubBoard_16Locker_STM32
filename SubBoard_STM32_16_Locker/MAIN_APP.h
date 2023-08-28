
#ifndef __MAIN_APP__
#define __MAIN_APP__

#include "STM32F1XX.h"

/*===================================================================
											CONFIG
===================================================================*/

#define PCF8575_ADDRESS_WRITE ( ((0x20u)<<(1u)) )
#define PCF8575_ADDRESS_READ ( ((0x27u)<<(1u)) )

#define TIME_LOCKER_OPEN (100u) /* Thoi gian cho kich relay */

/*=================================================================*/

/*===================================================================
											FUNCTION DECLARATIONS
===================================================================*/
void main_app(void);
/*=================================================================*/


#endif


