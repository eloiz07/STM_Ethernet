/* Define to prevent recursive inclusion -------------------------------------*/
#pragma once

/* Includes ------------------------------------------------------------------*/
#include "sDefine.h"
#include "sLED.h"

#include "w5500.h"
#include "socket.h"
#include "sSysTick.h"

#include "dhcp.h"

/* Defines  ------------------------------------------------------------------*/
#define SOCK_DHCP       6
#define SOCK_TCPS       0
#define PORT_TCPS		5000
#define DATA_BUF_SIZE   2048
#define MAX_DHCP_RETRY	2

/* Enum			   -----------------------------------------------------------*/

/* Define		   -----------------------------------------------------------*/
#define W5500_SPI_CS_PIN			GPIO_Pin_4
#define W5500_SPI_SCK_PIN			GPIO_Pin_5
#define W5500_SPI_MOSI_PIN			GPIO_Pin_6			// SDO		:  OUTPUT
#define W5500_SPI_MISO_PIN			GPIO_Pin_7			// SDI      :  INPUT
#define W5500_RESET					(GPIO_Pin_12 | GPIO_Pin_13)			// RESET

/* Exported variable ---------------------------------------------------------*/
extern u8 HW_INIT_FLAG;
extern wiz_NetInfo gWIZNETINFO;
extern u8 gDATABUF[];
extern u8 ET_INIT_FLAG;

/* Function		   -----------------------------------------------------------*/
void Init_W5500_SPI(void);
u8 wizchip_rw(u8 byte);
void wizchip_select(void);
void wizchip_deselect(void);
void wizchip_write(u8 wb);
u8 wizchip_read(void);
void wizchip_ipassign(void);
void wizchip_ipconflict(void);
void DHCP_REG_FUNC(void);
void W5500_Init(void);

void Net_Conf();
void display_Net_Info();
void Check_PHY(void);
void Check_DHCP(void);