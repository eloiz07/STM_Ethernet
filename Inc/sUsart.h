/* Define to prevent recursive inclusion -------------------------------------*/
#pragma once

/* Includes ------------------------------------------------------------------*/
#include "sDefine.h"

/* Enum			   -----------------------------------------------------------*/

/* Extern Variable -----------------------------------------------------------*/
extern BOOL _USART_State;

/* Function		   -----------------------------------------------------------*/
void SetUSART(void);
void OpenUSART(void);
int putchar(int ch);