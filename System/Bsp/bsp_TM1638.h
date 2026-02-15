#pragma once

#include "sys_main.h"

/* TM1638 Command Definitions */
#define TM1638_CMD_DISPLAY_ON   0x88
#define TM1638_CMD_BRIGHTNESS   0x07  /* OR with DISPLAY_ON: 0x88 ~ 0x8F */
#define TM1638_CMD_AUTO_ADDR    0x40
#define TM1638_CMD_FIXED_ADDR   0x44
#define TM1638_CMD_ADDR_BASE    0xC0

void TM1638_Init(void);
void TM1638_DisplayNumber(uint8_t position, uint8_t number);

void TM1638_SendByte_Single(uint8_t data);
void TM1638_SendByte(uint8_t *data, uint8_t size);
void TM1638_SendByte_DMA(uint8_t *data, uint8_t size);
void TM1638_ReceiveByte(void);
void TM1638_CallBack(SPI_HandleTypeDef *hspi);
