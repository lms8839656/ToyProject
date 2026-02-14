#include "main.h"

#include "sys_main.h"
#include "Bsp/bsp_TM1638.h"
#include "util/RTC.h"

void sys_main_task(void *arg)
{
    while(1)
    {
        HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);

        DisplayRTC();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
