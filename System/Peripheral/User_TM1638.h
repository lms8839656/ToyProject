#pragma once

#include "User_include.h"

extern void TM1638_SendByte(uint8_t data);
extern void TM1638_ReceiveByte(uint8_t data);
extern void TM1638_Init();
extern void TM1638_DisplayNumber(uint8_t position, uint8_t number);
extern void TM1638_CallBack(SPI_HandleTypeDef *hspi);

extern bool IsSended_SPI;


extern int TM1638_DisplayNumber_TEST(uint8_t position, uint8_t number);