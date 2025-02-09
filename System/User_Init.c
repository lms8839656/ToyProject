#include "main.h"

#include "User_include.h"

void User_SystemInit();

void User_SystemInit()
{
    /* Init User System */
    TM1638_Init();
    //TM1638_DisplayNumber(1, 3);
    extern void TM1638_GPIO_TEST();
    //TM1638_GPIO_TEST();
}