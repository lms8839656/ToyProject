#include "main.h"
#include "spi.h"
#include "sys_main.h"
#include "semphr.h"
#include "bsp_TM1638.h"

/* ───────────── Peripheral Config ───────────── */

#define TM1638_SPI          hspi5
#define SPI_DMA_TIMEOUT     pdMS_TO_TICKS(100)
#define TM1638_MAX_POSITION 7U

/* ───────────── 7-Segment Encoding ───────────── */

static const uint8_t TM1638_7Seg[10] =
{
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

/* ───────────── Private Variables ───────────── */

static SemaphoreHandle_t xSpiDmaSem = NULL;
ALIGN_32BYTES(static uint8_t dma_tx_buf[32]);

/* ───────────── STB Pin Control ───────────── */

static inline void STB_Low(void)
{
    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_RESET);
}

static inline void STB_High(void)
{
    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_SET);
}

/* ───────────── Low-Level SPI Transfer ───────────── */

void TM1638_SendByte_Single(uint8_t data)
{
    STB_Low();
    HAL_SPI_Transmit(&TM1638_SPI, &data, 1, HAL_MAX_DELAY);
    STB_High();
}

void TM1638_SendByte(uint8_t *data, uint8_t size)
{
    STB_Low();
    for(int i = 0; i < size; ++i)
    {
        HAL_SPI_Transmit(&TM1638_SPI, &data[i], 1, HAL_MAX_DELAY);
    }
    STB_High();
}

void TM1638_SendByte_DMA(uint8_t *data, uint8_t size)
{
    BaseType_t dma_done = pdFALSE;
    HAL_StatusTypeDef hal_status;

    if ((data == NULL) || (size == 0U) || (size > sizeof(dma_tx_buf)) || (xSpiDmaSem == NULL)) {
        return;
    }

    /* If previous transfer timed out, clear pending semaphore state before retry. */
    (void)xSemaphoreTake(xSpiDmaSem, 0U);
    memcpy(dma_tx_buf, data, size);
    STB_Low();
    SCB_CleanDCache_by_Addr((uint32_t*)dma_tx_buf, sizeof(dma_tx_buf));
    hal_status = HAL_SPI_Transmit_DMA(&TM1638_SPI, dma_tx_buf, size);
    if (hal_status != HAL_OK) {
        STB_High();
        return;
    }

    dma_done = xSemaphoreTake(xSpiDmaSem, SPI_DMA_TIMEOUT);
    STB_High();
    if (dma_done != pdTRUE) {
        (void)HAL_SPI_Abort(&TM1638_SPI);
    }
}

void TM1638_ReceiveByte(void)
{
    uint8_t receivedData = 0;
    HAL_SPI_Receive(&TM1638_SPI, &receivedData, 1, HAL_MAX_DELAY);
}

/* ───────────── ISR Callback ───────────── */

void TM1638_CallBack(SPI_HandleTypeDef *hspi)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if ((hspi == NULL) || (hspi->Instance != SPI5) || (xSpiDmaSem == NULL)) {
        return;
    }

    xSemaphoreGiveFromISR(xSpiDmaSem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* ───────────── Display API ───────────── */

void TM1638_DisplayNumber(uint8_t position, uint8_t number)
{
    uint8_t data[2];
    if ((position > TM1638_MAX_POSITION) || (number > 9U)) {
        return;
    }

    data[0] = TM1638_CMD_ADDR_BASE | (uint8_t)(position * 2U);
    data[1] = TM1638_7Seg[number];
    TM1638_SendByte_DMA(data, 2);
}

/* ───────────── Initialization ───────────── */

void TM1638_Init(void)
{
    xSpiDmaSem = xSemaphoreCreateBinary();
    if (xSpiDmaSem == NULL) {
        return;
    }

    /* Display ON, Max Brightness */
    TM1638_SendByte_Single(TM1638_CMD_DISPLAY_ON | TM1638_CMD_BRIGHTNESS);

    /* Auto Address Increment Mode */
    TM1638_SendByte_Single(TM1638_CMD_AUTO_ADDR);

    uint8_t data[17] = {0};
    data[0] = TM1638_CMD_ADDR_BASE;

    /* All Segments ON */
    memset(&data[1], 0xFF, 16);
    TM1638_SendByte(data, sizeof(data));

    /* All Segments OFF */
    memset(&data[1], 0x00, 16);
    TM1638_SendByte(data, sizeof(data));
}
