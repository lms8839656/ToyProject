#include "main.h"
#include "tim.h"

#include "User_include.h"

void User_SystemInit();

void User_SystemInit()
{
    /* Init User System */
    TM1638_Init();
    //TM1638_GPIO_TEST();

    /* Init User LCD */
    htim2.Instance->CCR1 = 90;
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    
}