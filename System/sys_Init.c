#include "main.h"
#include "tim.h"

#include "sys_main.h"
#include "Bsp/bsp_TM1638.h"

volatile bool kernelStarted = false;

static void sys_init_task(void *arg);

void User_SystemInit(void)
{
    xTaskCreate(sys_init_task, "initTask", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
}

static void sys_init_task(void *arg)
{
    /* Init User System */
    TM1638_Init();

    /* Init User LCD */
    htim2.Instance->CCR1 = 90;
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

    /* Kernel is now running */
    kernelStarted = true;

    /* Create application tasks */
    xTaskCreate(sys_main_task, "sysTask", 256, NULL, tskIDLE_PRIORITY + 1, NULL);

    /* Init complete, delete this task */
    vTaskDelete(NULL);
}
