
/*===================================================================
											INCLUDE FILE
===================================================================*/
#include "MAIN_APP.h"
/*=================================================================*/

/*===================================================================
											TYPEDEF & ENUM
===================================================================*/

typedef enum 
{
	READ_LOCKER,
	OPEN_LOCKER
}Action_enum;

typedef enum 
{
	ACTION_OK,
	ACTION_ERROR
}Action_State_enum;

typedef enum 
{
	NOT_SUCCESS,
	SUCCESS
}StateLocker_enum;

typedef struct
{
	unsigned char Master_Address;
	unsigned char Slave_Address;
	unsigned char ID_Locker;
	unsigned char Action;
	unsigned char State_Locker;
	char dummy[3];
}LockerControl_Typedef;

/*=================================================================*/

/*===================================================================
											VARIABLE DECLARATION
===================================================================*/
static unsigned char Board_Address=0;
static LockerControl_Typedef LOCKER_CONTROL;
static volatile unsigned char Step_Action=0;
static unsigned char Buffer_Action[UART_SIZE_BUFFER]={0};
static unsigned char CODE_ERROR=0;
static unsigned char COUNT_ERROR=0;
/*=================================================================*/

/*===================================================================
											FUNCTIONS
===================================================================*/

static void MAIN_INIT(void)
{
	LOCKER_CONTROL.Master_Address=0;
	LOCKER_CONTROL.Slave_Address=0;
	LOCKER_CONTROL.State_Locker=0;
	LOCKER_CONTROL.ID_Locker=0;
	LOCKER_CONTROL.Action=0;

	Board_Address=0;
	Step_Action=0;

	BufferClear(Buffer_Action, sizeof(Buffer_Action));
	BufferClear(UART_BufferRead, sizeof(UART_BufferRead));

}

/* 1-PA3, 2-PA4, 3-PA6, 4-PA7 */
static void Config_Switch_Address(void)
{
	Enable_Disable_Clock_PortA(Enable);
	SetPinInput(PORTA,PIN3,InputPullUp_PullDown,PullUp);
	SetPinInput(PORTA,PIN4,InputPullUp_PullDown,PullUp);
	SetPinInput(PORTA,PIN6,InputPullUp_PullDown,PullUp);
	SetPinInput(PORTA,PIN7,InputPullUp_PullDown,PullUp);
}

/* Get Board Address: 0-15 */
static void Get_Board_Address(void)
{
	Config_Switch_Address();
	if(ReadPin(PORTA,PIN3)!=0) Board_Address |= 1u<<0;
	if(ReadPin(PORTA,PIN4)!=0) Board_Address |= 1u<<1;
	if(ReadPin(PORTA,PIN6)!=0) Board_Address |= 1u<<2;
	if(ReadPin(PORTA,PIN7)!=0) Board_Address |= 1u<<3;
}

/* Locker_ID: 1-16 */
static unsigned char Locker_Open(unsigned char Locker_ID)
{
	unsigned char State= ACTION_OK;

	if( PCF8575_WritePin(PCF8575_ADDRESS_WRITE,Locker_ID,HIGH)==PCF8575_OK )
	{
		Delay_TIM2_ms(TIME_LOCKER_OPEN);
		if( PCF8575_WritePin(PCF8575_ADDRESS_WRITE,Locker_ID,LOW)==PCF8575_OK )
		{
			Delay_TIM2_ms(TIME_LOCKER_OPEN);
		} else State= ACTION_ERROR;
	}
	else State= ACTION_ERROR;

	return State;
}

/* Locker_ID: 1-16 */
static unsigned char Locker_GetState(unsigned char *StateLocker, unsigned char Locker_ID)
{
	unsigned char State= ACTION_OK;

	if( PCF8575_ReadPin(StateLocker,PCF8575_ADDRESS_READ,Locker_ID)==PCF8575_ERROR )
	{
		State= ACTION_ERROR;
	}

	return State;
}

static void LinkCharac(unsigned char *Des, unsigned char *a, unsigned char *b)
{
	*Des = ( ((*a) * 10) + (*b) );
}

static void RS485_Read(void)
{
	if(UART_BufferRead[0]==91 && UART_BufferRead[9]==93) /* [ ] */
	{
		unsigned char CheckRule=0;
		unsigned char i=1;

		/* Chuyen ASCII ve thap phan */
		for(i=1; i<=8; i++)
		{
			Buffer_Action[i] = (UART_BufferRead[i]-'0');
		}

		/* Kiem tra Frame nhan */
		for(i=1; i<=8; i++)
		{
			if( (Buffer_Action[i]>0) && (Buffer_Action[i]<=9) )
			{
				continue;
			}
			else /* Frame loi */
			{
				CheckRule=1;
				break;
			}
		}

		if(CheckRule==1) /* Frame loi */
		{
			Step_Action = 0;
			BufferClear(Buffer_Action, sizeof(Buffer_Action));
			BufferClear(UART_BufferRead, sizeof(UART_BufferRead));
		}
		else 
		{
			LinkCharac(&LOCKER_CONTROL.Master_Address, &Buffer_Action[1], &Buffer_Action[2]);
			LinkCharac(&LOCKER_CONTROL.Slave_Address, &Buffer_Action[3], &Buffer_Action[4]);
			LinkCharac(&LOCKER_CONTROL.ID_Locker, &Buffer_Action[5], &Buffer_Action[6]);
			LinkCharac(&LOCKER_CONTROL.Action, &Buffer_Action[7], &Buffer_Action[8]);

			Step_Action = 1;
			BufferClear(UART_BufferRead, sizeof(UART_BufferRead));
		}

		BufferClear(UART_BufferRead, sizeof(UART_BufferRead));

	}
	
}

static void RS485_SendState(void)
{
	unsigned char i=1;
	Buffer_Action[0]='[';
	Buffer_Action[9]=']';	

	/* chuyen thap phan ve ASCII */
	for(i=1; i<=6; i++)
	{
		Buffer_Action[i] += '0';
	}

	if(LOCKER_CONTROL.State_Locker == SUCCESS)
	{
		Buffer_Action[7]='0';
		Buffer_Action[8]='1';
	}
	else 
	{
		Buffer_Action[7]='0';
		Buffer_Action[8]='0';
	}	

	USART1_SendMultiData(Buffer_Action, sizeof(Buffer_Action));
	Delay_TIM2_ms(100);
	USART1_SendMultiData(Buffer_Action, sizeof(Buffer_Action));
	Delay_TIM2_ms(100);
	USART1_SendMultiData(Buffer_Action, sizeof(Buffer_Action));									
}

/* ERR: 1-IC WRITE ; 2-IC READ */
static void RS485_SendERR(char ERR)
{
	Buffer_Action[0]='[';
	Buffer_Action[9]=']';	

	Buffer_Action[1]='E';
	Buffer_Action[2]='R';
	Buffer_Action[3]='R';
	Buffer_Action[4]='O';
	Buffer_Action[5]='R';
	Buffer_Action[6]='I';
	Buffer_Action[7]='C';

	if(ERR == 1) /* Loi IC Write */
	{
		Buffer_Action[8]='W';
	}
	else /* Loi IC Read */
	{
		Buffer_Action[8]='R';
	}

	USART1_SendMultiData(Buffer_Action, sizeof(Buffer_Action));
	Delay_TIM2_ms(100);
	USART1_SendMultiData(Buffer_Action, sizeof(Buffer_Action));
	Delay_TIM2_ms(100);
	USART1_SendMultiData(Buffer_Action, sizeof(Buffer_Action));		
}

static void Locker_Action(void)
{
	RS485_Read();

	switch (Step_Action)
	{
		case 1:

			if( LOCKER_CONTROL.Slave_Address == Board_Address) /* Dung dia chi */
			{		
				if( LOCKER_CONTROL.Action == OPEN_LOCKER)  /* go to B2: Locker Open */
				{
					Step_Action = 2;
				}
				else  /* go to B3: Read State */
				{
					Step_Action = 3;
				}
			}	
			else
			{
				Step_Action = 0;
				BufferClear(UART_BufferRead, sizeof(UART_BufferRead));
			} 

			break;
		case 2: /* Mo cua locker */

			if( Locker_Open( LOCKER_CONTROL.ID_Locker )== ACTION_ERROR) CODE_ERROR=1;
			if( Locker_Open( LOCKER_CONTROL.ID_Locker )== ACTION_ERROR) CODE_ERROR=1;
			if( Locker_Open( LOCKER_CONTROL.ID_Locker )== ACTION_ERROR) CODE_ERROR=1;

			if( CODE_ERROR==0 ) Step_Action = 3;	
			else Step_Action = 4;

			break;		
		case 3: /* Get status locker and send to master */

			if( Locker_GetState(&LOCKER_CONTROL.State_Locker, LOCKER_CONTROL.ID_Locker)==ACTION_ERROR )
			{
				CODE_ERROR=2;
				Step_Action = 4;
			}
			else 
			{
				RS485_SendState();
				Step_Action = 5;
			}

			break;
		case 4: /* SEND ERROR */

			RS485_SendERR(CODE_ERROR);

			/* Neu loi 3 lan thi Reset Chip */
			if( COUNT_ERROR<3 ) COUNT_ERROR++;
			else NVIC_SystemReset();

			CODE_ERROR=0;
			Step_Action=0;
			BufferClear(Buffer_Action, sizeof(Buffer_Action));
			BufferClear(UART_BufferRead, sizeof(UART_BufferRead));

			break;		

		case 5: /* ket thuc qua trinh */

			CODE_ERROR=0;
			Step_Action=0;
			BufferClear(Buffer_Action, sizeof(Buffer_Action));
			BufferClear(UART_BufferRead, sizeof(UART_BufferRead));

			break;									
		
		default:
			break;
	}
}
/*=================================================================*/


/*===================================================================
															MAIN APP
===================================================================*/
void main_app(void)
{
	InitClockHSE();
	USART1_Setup();
	Init_TIM2_Delay();
	MAIN_INIT();
	Get_Board_Address();
	I2C1_Init(I2C_FAST);

	/* Đảm bảo khi khởi động lại các ô tủ không tự mở */
	PCF8575_WriteALL(PCF8575_ADDRESS_WRITE, LOW);

	/* Kiem tra ket noi IC PCF8575 WRITE */
	while( I2C1_Check_Device(PCF8575_ADDRESS_WRITE)==I2C_TIMEOUT )
	{
		RS485_SendERR(1);
		Delay_TIM2_ms(5000);
	}
	/* Kiem tra ket noi IC PCF8575 READ */
	while( I2C1_Check_Device(PCF8575_ADDRESS_READ)==I2C_TIMEOUT )
	{
		RS485_SendERR(2);
		Delay_TIM2_ms(5000);
	}

	while (1)
	{		

		Locker_Action();

	}
}
/*=================================================================*/

