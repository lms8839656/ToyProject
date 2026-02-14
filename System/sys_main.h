#pragma once

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

extern volatile bool kernelStarted;

extern void User_SystemInit(void);
extern void sys_main_task(void *arg);
