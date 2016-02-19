/* Includes ------------------------------------------------------------------*/
#include "sUsart.h"

/* Variable ------------------------------------------------------------------*/
BOOL	_USART_State = FALSE;
u32		_USART_BaudRate = 115200;

/* Function ------------------------------------------------------------------*/
void SetUSART(void)
{
	USART_InitTypeDef       USART_InitStructure;
	NVIC_InitTypeDef        NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// USART TX
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	// USART RX
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	// USART Configuration
	USART_InitStructure.USART_BaudRate = _USART_BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
	_USART_State = TRUE;
}

void OpenUSART(void)
{
	// close serial port
	if(_USART_State == TRUE)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
	
	// open serial port
	SetUSART();
}

int putchar(int ch)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1,ch);
	return ch;
}