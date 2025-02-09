#include "main.h"
#include "spi.h"

/* Define Peripheral */


#define TM1638_SPI      hspi5

const unsigned char TM1636_7Seg[10] =
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

uint8_t txBuffer[2];

int flag = 0;

void TM1638_SendByte(uint8_t data);
void TM1638_ReceiveByte(uint8_t data);
void TM1638_Init();
void TM1638_DisplayNumber(uint8_t position, uint8_t number);

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) /* Derogation MISRAC2012-Rule-8.13 */
{
  /* Prevent unused argument(s) compilation warning */
  if (hspi->Instance == SPI4) {
    // 전송 완료 시 실행할 코드 (예: LED 점멸)
    flag = 1;
    }

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxCpltCallback should be implemented in the user file
   */
}


void TM1638_SendByte(uint8_t data)
{
    
    HAL_SPI_Transmit(&TM1638_SPI, &data, 1, HAL_MAX_DELAY);
    
}

void TM1638_SendByte_DMA(uint8_t data)
{
    flag = 0;
    txBuffer[0] = data;

    HAL_SPI_Transmit_DMA(&TM1638_SPI, txBuffer, 1);
    while (!flag)
    {

    }
}

void TM1638_ReceiveByte(uint8_t data)
{
    uint8_t receivedData = 0;
    HAL_SPI_Receive(&TM1638_SPI, &receivedData, 1, HAL_MAX_DELAY);
    return receivedData;
}

void TM1638_Init()
{
    // 2. 디스플레이 및 밝기 설정 (0x8F = 밝기 최대)
    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_RESET);
    TM1638_SendByte(0x8F);  
    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_SET);
    // 3. 데이터 설정 (0x40 = 자동 주소 증가 모드)
    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_RESET);
    TM1638_SendByte(0x40);
    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_RESET);
    TM1638_SendByte(0xC0);  // 주소 0번부터 시작
    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_RESET);
    for (int i = 0; i < 16; i++) {
        
        TM1638_SendByte(0x00);
        
        HAL_Delay(10);
    }
    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_RESET);
    for (int i = 0; i < 16; i++) {
        
        TM1638_SendByte(0xFF);
        
        HAL_Delay(10);
    }
    TM1638_SendByte(0xFF);
    HAL_GPIO_WritePin(SPI_STB_GPIO_Port, SPI_STB_Pin, GPIO_PIN_SET);
}

void TM1638_DisplayNumber(uint8_t position, uint8_t number)
{
    TM1638_SendByte(0x44);
    TM1638_SendByte(0xC0 | (position * 2));
    TM1638_SendByte(TM1636_7Seg[number]);
}


#define TM1638_STB_PIN    GPIO_PIN_7
#define TM1638_STB_PORT   GPIOF

#define TM1638_CLK_PIN    GPIO_PIN_8
#define TM1638_CLK_PORT   GPIOF

#define TM1638_DIO_PIN    GPIO_PIN_9
#define TM1638_DIO_PORT   GPIOF

void TM1638_SendByte_TEST(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(TM1638_CLK_PORT, TM1638_CLK_PIN, GPIO_PIN_RESET); // CLK LOW
        HAL_GPIO_WritePin(TM1638_DIO_PORT, TM1638_DIO_PIN, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_Delay(10); // 약간의 지연
        HAL_GPIO_WritePin(TM1638_CLK_PORT, TM1638_CLK_PIN, GPIO_PIN_SET);   // CLK HIGH
        data >>= 1;
    }
}

void TM1638_Clear_TSET(void) {
    HAL_GPIO_WritePin(TM1638_STB_PORT, TM1638_STB_PIN, GPIO_PIN_RESET);
    TM1638_SendByte_TEST(0xC0);  // 0번 주소부터 데이터 전송 시작

    for (int i = 0; i < 16; i++) {
        HAL_Delay(10);
        TM1638_SendByte_TEST(0xFF);  // 모든 데이터 0으로 설정
    }

    HAL_GPIO_WritePin(TM1638_STB_PORT, TM1638_STB_PIN, GPIO_PIN_SET);

    HAL_GPIO_WritePin(TM1638_STB_PORT, TM1638_STB_PIN, GPIO_PIN_RESET);
    TM1638_SendByte_TEST(0xC0 | (3 * 2));
    TM1638_SendByte_TEST(TM1636_7Seg[3]);
    HAL_GPIO_WritePin(TM1638_STB_PORT, TM1638_STB_PIN, GPIO_PIN_SET);
}

void TM1638_GPIO_TEST()
{

    HAL_GPIO_WritePin(TM1638_STB_PORT, TM1638_STB_PIN, GPIO_PIN_RESET);
    TM1638_SendByte_TEST(0x8F); // TM1638 활성화 (밝기 최대)
    HAL_GPIO_WritePin(TM1638_STB_PORT, TM1638_STB_PIN, GPIO_PIN_SET);
    TM1638_Clear_TSET();
}

