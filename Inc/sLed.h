/* Define to prevent recursive inclusion -------------------------------------*/
#pragma once

/* Includes ------------------------------------------------------------------*/
#include "sDefine.h"

/* Enum			   -----------------------------------------------------------*/
typedef enum
{
	LED1, LED2
} LED;

/* Extern Variable -----------------------------------------------------------*/

/* Function		   -----------------------------------------------------------*/
void SetGPIO_LED(void);
BOOL SetLedState(const LED no, const BOOL state);
BOOL GetLedState(const LED no);
BOOL ToggleLedState(const LED no);