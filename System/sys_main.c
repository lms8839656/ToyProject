#include "main.h"

#include "sys_main.h"
#include "Bsp/bsp_TM1638.h"
#include "util/RTC.h"

static void sys_main_task(void *arg);

void User_RunSystem(void)
{
    xTaskCreate(sys_main_task, "sysTask", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
}

static void sys_main_task(void *arg)
{
    while(1)
    {
        HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);

        DisplayRTC();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
