
#ifndef _NVIC_Register_
#define _NVIC_Register_

#include "NVIC.h"

typedef struct 
{
  volatile unsigned int ISER[8u];        
	unsigned int RESERVED_0[24];
  volatile unsigned int ICER[8u];       
  unsigned int RESERVED_1[24];
  volatile unsigned int ISPR[8u];       
  unsigned int RESERVED_2[24];
  volatile unsigned int ICPR[8u];       
  unsigned int RESERVED_3[24];
  volatile unsigned int IABR[8u];       
  unsigned char RESERVED_4[224u];
  volatile unsigned int IPR[60u];      
  unsigned char RESERVED_5[2573u];
  volatile unsigned int STIR;          
} NVIC_Typedef;

#define NVIC_BASE_ADDRESS   ((unsigned int)0xE000E100u)
#define NVIC 				        ((NVIC_Typedef*)NVIC_BASE_ADDRESS)


typedef struct
{
  unsigned int CPUID;                        
  unsigned int ICSR;                         
  unsigned int VTOR;                         
  unsigned int AIRCR;                        
  unsigned int SCR;                          
  unsigned int CCR;                          
  unsigned char SHP[12];                   
  unsigned int SHCSR;                      
  unsigned int CFSR;                       
  unsigned int HFSR;                        
  unsigned int DFSR;                        
  unsigned int MMFAR;                       
  unsigned int BFAR;                        
  unsigned int AFSR;                        
  unsigned int PFR[2];                       
  unsigned int DFR;                          
  unsigned int ADR;                          
  unsigned int MMFR[4];                      
  unsigned int ISAR[5];                      
} SCB_Type;

#define SCS_BASE   (0xE000E000u)                           
#define SCB_BASE   (SCS_BASE + 0x0D00u)                   
#define SCB        ((SCB_Type *)SCB_BASE)      

#define NVIC_AIRCR_VECTKEY    (0x5FA << 16u)
#define NVIC_SYSRESETREQ            (2u)    

#endif


