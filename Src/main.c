/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Variable ------------------------------------------------------------------*/
u8 HW_INIT_FLAG = FALSE;
u8 PHY_INIT_FLAG = FALSE;

u8 isModeSelect = FALSE;
u8 isErrorCount = 0;
u32 ModeCount = 0;
/* Function ------------------------------------------------------------------*/
void main(void)
{
	// clock source init
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

  	// USART init
  	OpenUSART();

  	// LED init
  	SetGPIO_LED();

  	// KEY init
  	SetGPIO_KEY();

    // Interrupt init
    RegisterInterruptHandler();

  	// W5500 SPI init
  	Init_W5500_SPI();

  	// HW init end
  	DelayMs(1);		HW_INIT_FLAG = TRUE;

  	// W5500 init
  	W5500_Init();
  	Net_Conf();

  	// Ethernet MODE selection
  	while(TRUE)
  	{
  		u8 tmp = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
  		if(isModeSelect == tmp)
  		{
  			ModeCount++;
  		}
  		else
  		{
  			isModeSelect = tmp;
  			ModeCount = 0;
  		}

  		if(ModeCount < 10)
  			DelayMs(1);
  		else
  			break;
  	}
    isModeSelect = FALSE;

  	if(isModeSelect == TRUE)
  	{
  		// DHCP mode
  		if(gWIZNETINFO.dhcp == NETINFO_DHCP)
  		{
  			DHCP_init(SOCK_DHCP, gDATABUF);
        DHCP_REG_FUNC();
        //DHCP_TimerInit();

  			ET_INIT_FLAG = FALSE;
  		}
  	}
  	else
  	{
  		// Fixed mode
  		gWIZNETINFO.dhcp = NETINFO_STATIC;
  		Net_Conf();
  		display_Net_Info();

  		ET_INIT_FLAG = TRUE;
      SetLedState(LED1, TRUE);
  	}

    uint8_t tmpstr[6] = {0,};
  	ctlwizchip(CW_GET_ID,(void*)tmpstr);

    printf("\r\n=======================================\r\n");
	  printf(" WIZnet %s Develop Board v%d.%.2d\r\n", tmpstr, VER_H, VER_L);
	  printf("=======================================\r\n");

    // interrupt mask setting
    setSIMR(0x01);
    setSn_IMR(0x00, 0x04);
    // interrupt register init
    setSn_IR(0x00, 0x1F);

  	while(TRUE)
  	{
  		if(PHY_INIT_FLAG == FALSE)
  		{
  			Check_PHY();
  			PHY_INIT_FLAG = TRUE;
  		}

  		if(gWIZNETINFO.dhcp == NETINFO_DHCP)
  		{
  			Check_DHCP();
  		}

  		if(ET_INIT_FLAG == TRUE)
  		{
  			int32_t result = 0;
  			if((result = loopback_tcps(SOCK_TCPS, gDATABUF, PORT_TCPS)) < 0)
  			{
  				printf("SOCKET_ERROR: %ld\r\n", result);

  				if(++isErrorCount > 20)
  					break;
  			}
  		}
  	}
}

int32_t checkEthernet(u8 sn, u8* buf, u16 port)
{
  int32_t ret = -1;
  switch(getSn_SR(sn))
  {
    case SOCK_INIT:
      if((ret = listen(sn)) != SOCK_OK) return ret;
      //printf("%d:Listen, TCP server loopback, port [%d]\r\n", sn, port);
      break;
    case SOCK_CLOSE_WAIT:
      if((ret = disconnect(sn)) != SOCK_OK) return ret;
      //printf("%d:CloseWait\r\n",sn);
      break;
    case SOCK_CLOSED:
      if((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
      //printf("%d:Socket Closed\r\n", sn);
      break;
    case SOCK_TIME_WAIT:
      //printf("%d:Socket Time wait\r\n", sn);
      break;
    case SOCK_ESTABLISHED:
      // check Connected
      if(getSn_IR(sn) & Sn_IR_CON)
      {
        //u8 destip[4];
        //getSn_DIPR(sn, destip);
        //u16 destport = getSn_DPORT(sn);

        //printf("%d:Connected - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
        setSn_IR(sn, Sn_IR_CON);
      }
      break;
  }

  return 1;
}

void DHCP_TimerInit(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
     
    /* TIM2 Clock Enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
     
    /* Enable TIM2 Global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
     
    /* TIM2 Initialize */   
    TIM_TimeBaseStructure.TIM_Period=1000-1; // 1kHz
    TIM_TimeBaseStructure.TIM_Prescaler=24-1; // 1MHz
    TIM_TimeBaseStructure.TIM_ClockDivision=0;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
     
    /* TIM2 Enale */
    TIM_Cmd(TIM2,ENABLE);
    TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE); // interrupt enable
}

void RegisterInterruptHandler(void)
{ 
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);
 
  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  EXTI_ClearITPendingBit(EXTI_Line2);
}