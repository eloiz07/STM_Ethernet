/* Includes ------------------------------------------------------------------*/
#include "sSysTick.h"

/* Variable ------------------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/
void DelayUs(const u32 usec)
{
	RCC_ClocksTypeDef RCC_Clocks;

	/* Configure HCLK clock as SysTick clock source */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    RCC_GetClocksFreq(&RCC_Clocks);

    // Set SysTick Reload(1us) register and Enable
    // usec * (RCC_Clocks.HCLK_Frequency / 1000000) < 0xFFFFFFUL -- because of 24bit timer
    // RCC_Clocks.HCLK_Frequency = 72000000 
    // Systick Reload Value Register = 72
    // 72 / 72000000 = 1us
    SysTick_Config(usec * (RCC_Clocks.HCLK_Frequency / 1000000));
    
    // SysTick Interrupt Disable
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk ;

    // Until Tick count is 0
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
}

void DelayMs(const u32 msec)
{
    DelayUs(1000 * msec);
}