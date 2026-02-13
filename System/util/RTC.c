#include "main.h"
#include "rtc.h"
#include "../Bsp/bsp_TM1638.h"

extern RTC_HandleTypeDef hrtc;
RTC_DateTypeDef g_Date;
RTC_TimeTypeDef g_Time;

void InitRTC()
{

}

void GetRTC()
{
    HAL_RTC_GetTime(&hrtc, &g_Time, RTC_FORMAT_BCD);
    HAL_RTC_GetDate(&hrtc, &g_Date, RTC_FORMAT_BCD);
}

void DisplayRTC()
{
    GetRTC();
    TM1638_DisplayNumber(0, (g_Time.Hours >> 4) & 0x0F);
    TM1638_DisplayNumber(1, g_Time.Hours & 0x0F);
    TM1638_DisplayNumber(2, (g_Time.Minutes >> 4) & 0x0F);
    TM1638_DisplayNumber(3, g_Time.Minutes & 0x0F);
    TM1638_DisplayNumber(4, (g_Time.Seconds >> 4) & 0x0F);
    TM1638_DisplayNumber(5, g_Time.Seconds & 0x0F);
}
