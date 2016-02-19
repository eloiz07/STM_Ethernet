/**
  ******************************************************************************
  * @file    Project/Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.1.2
  * @date    09/28/2009
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

void USART1_IRQHandler(void)
{
  char receive_data;

  // receive
  while(USART_GetITStatus(USART1, USART_IT_RXNE) == RESET);
  receive_data = USART_ReceiveData(USART1) & 0xFF;
  USART_SendData(USART1, receive_data);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET ); 

  USART_ClearITPendingBit(USART1, USART_IT_RXNE); 
}

void SPI1_IRQHandler(void)
{
  
}

void EXTI2_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line2) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line2);

    u8 snir = getSn_IR(0x00);
    // Sn_IR_CON
    if( ((snir >> 0) & 0x01) != FALSE )
      printf("Sn_IR_CON\r\n");
      //setSn_IR(SOCK_TCPS, 0x01);

    // Sn_IR_DISCON
    if( ((snir >> 1) & 0x01) != FALSE )
      printf("Sn_IR_DISCON\r\n");
      //setSn_IR(SOCK_TCPS, 0x02);

    // Sn_IR_RECV
    if( ((snir >> 2) & 0x01) != FALSE )
      printf("Sn_IR_RECV\r\n");
      //setSn_IR(SOCK_TCPS, 0x04);

    // Sn_IR_TIMEOUT
    if( ((snir >> 3) & 0x01) != FALSE )
      printf("Sn_IR_TIMEOUT\r\n");
      //setSn_IR(SOCK_TCPS, 0x08);

    // Sn_IR_SENDOK
    if( ((snir >> 4) & 0x01) != FALSE )
      printf("Sn_IR_SENDOK\r\n");
      //setSn_IR(SOCK_TCPS, 0x10);

    setSn_IR(0x00, 0x1F);
  }
}

void EXTI15_10_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line13) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line13);
  }
  if(EXTI_GetITStatus(EXTI_Line12) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line12);
  }  
}

void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // Clear the interrupt flag
        DHCP_time_handler();
    }
}