#include "main.h"
#include "spi.h"
#include "User_include.h"
#include "User_TM1638.h"



void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) /* Derogation MISRAC2012-Rule-8.13 */
{
  /* Prevent unused argument(s) compilation warning */
    if (hspi->Instance == SPI5) {
        TM1638_CallBack(hspi);
    }
    
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxCpltCallback should be implemented in the user file
   */
}