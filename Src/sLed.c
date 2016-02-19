/* Includes ------------------------------------------------------------------*/
#include "sLed.h"

/* Variable ------------------------------------------------------------------*/
BOOL	LED_State[4] = {FALSE, FALSE, FALSE, FALSE};

/* Function ------------------------------------------------------------------*/
BOOL ToggleLedState(const LED no)
{
	if(no > (sizeof(LED_State) / sizeof(LED_State[0])))
		return FALSE;

	LED_State[no] = LED_State[no] == FALSE ? TRUE : FALSE;

	if(LED_State[no] == TRUE) GPIO_SetBits(GPIOB, GPIO_Pin_10 * (no + 1));
	else					  GPIO_ResetBits(GPIOB, GPIO_Pin_10 * (no + 1));

	return TRUE;
}

BOOL GetLedState(const LED no)
{
	BOOL state = FALSE;

	if(no > (sizeof(LED_State) / sizeof(LED_State[0])))
		return state;

	state = LED_State[no];

	return state;
}

BOOL SetLedState(const LED no, const BOOL state)
{
	if(no > (sizeof(LED_State) / sizeof(LED_State[0])))
		return FALSE;

	LED_State[no] = state;
	if(state == TRUE) GPIO_SetBits(GPIOB, GPIO_Pin_10 * (no + 1));
	else			  GPIO_ResetBits(GPIOB, GPIO_Pin_10 * (no + 1));

	return TRUE;
}

void SetGPIO_LED(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	// LED
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}