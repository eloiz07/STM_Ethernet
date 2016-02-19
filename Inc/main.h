/* Define to prevent recursive inclusion -------------------------------------*/
#pragma once

/* Includes ------------------------------------------------------------------*/
#include "sDefine.h"

// H/W
#include "sLed.h"
#include "sKey.h"
#include "sUsart.h"
#include "sSysTick.h"

// W5500
#include "sW5500_SPI.h"
#include "loopback.h"

#define VER_H		1
#define VER_L		00

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variable ---------------------------------------------------------*/
extern u8 HW_INIT_FLAG;
extern wiz_NetInfo gWIZNETINFO;

/* Exported functions ------------------------------------------------------- */
void DHCP_TimerInit(void);
void RegisterInterruptHandler(void);
int32_t checkEthernet(u8 sn, u8* buf, u16 port);