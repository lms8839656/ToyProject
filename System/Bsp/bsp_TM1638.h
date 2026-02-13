#pragma once

#include "sys_main.h"

extern void TM1638_SendByte_Single(uint8_t data);
extern void TM1638_SendByte(uint8_t *data, uint8_t size);
extern void TM1638_SendByte_DMA(uint8_t *data, uint8_t size);
extern void TM1638_ReceiveByte(uint8_t data);
extern void TM1638_Init();
extern void TM1638_DisplayNumber(uint8_t position, uint8_t number);
extern void TM1638_CallBack(SPI_HandleTypeDef *hspi);
