#include "main.h"
#include "tim.h"

#include "sys_main.h"
#include "Bsp/bsp_TM1638.h"

void User_SystemInit(void)
{
    /* Init User System */
    TM1638_Init();

    /* Init User LCD */
    htim2.Instance->CCR1 = 90;
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}
