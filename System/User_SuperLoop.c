#include "main.h"

#include "User_include.h"
#include "User_TM1638.h"
#include "User_RTC.h"

void User_RunSystem();

void User_RunSystem()
{
    while(1)
    {
        HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
        
        DisplayRTC();
        HAL_Delay(1000);
        continue;
    }
}
