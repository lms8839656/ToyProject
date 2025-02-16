#include "main.h"
#include "rtc.h"
#include "User_TM1638.h"

extern RTC_HandleTypeDef hrtc;
RTC_DateTypeDef g_Date;
RTC_TimeTypeDef g_Time;

#define TM1638_STB_PIN    GPIO_PIN_6
#define TM1638_STB_PORT   GPIOA

#define TM1638_CLK_PIN    GPIO_PIN_5
#define TM1638_CLK_PORT   GPIOA

#define TM1638_DIO_PIN    GPIO_PIN_5
#define TM1638_DIO_PORT   GPIOB

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
    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_RESET);
    TM1638_DisplayNumber(0, (g_Time.Hours >> 4) & 0x0F);
    TM1638_DisplayNumber(1, g_Time.Hours & 0x0F);
    TM1638_DisplayNumber(2, (g_Time.Minutes >> 4) & 0x0F);
    TM1638_DisplayNumber(3, g_Time.Minutes & 0x0F);
    TM1638_DisplayNumber(4, (g_Time.Seconds >> 4) & 0x0F);
    TM1638_DisplayNumber(5, g_Time.Seconds & 0x0F);
    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_SET);
}

void DisplayRTC_TEST()
{
    GetRTC();
    HAL_GPIO_WritePin(TM1638_STB_PORT, TM1638_STB_PIN, GPIO_PIN_RESET);
    TM1638_DisplayNumber_TEST(0, (g_Time.Hours >> 4) & 0x0F);
    HAL_Delay(10);
    TM1638_DisplayNumber_TEST(1, g_Time.Hours & 0x0F);
    HAL_Delay(10);
    TM1638_DisplayNumber_TEST(2, (g_Time.Minutes >> 4) & 0x0F);
    HAL_Delay(10);
    TM1638_DisplayNumber_TEST(3, g_Time.Minutes & 0x0F);
    HAL_Delay(10);
    TM1638_DisplayNumber_TEST(4, (g_Time.Seconds >> 4) & 0x0F);
    HAL_Delay(10);
    TM1638_DisplayNumber_TEST(5, g_Time.Seconds & 0x0F);
    HAL_Delay(10);
    HAL_GPIO_WritePin(TM1638_STB_PORT, TM1638_STB_PIN, GPIO_PIN_SET);
}
