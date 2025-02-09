################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../System/User_Init.c \
../System/User_MSP2404.c \
../System/User_SuperLoop.c \
../System/User_TM1638.c 

OBJS += \
./System/User_Init.o \
./System/User_MSP2404.o \
./System/User_SuperLoop.o \
./System/User_TM1638.o 

C_DEPS += \
./System/User_Init.d \
./System/User_MSP2404.d \
./System/User_SuperLoop.d \
./System/User_TM1638.d 


# Each subdirectory must supply rules for building sources it contributes
System/%.o System/%.su System/%.cyclo: ../System/%.c System/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../System -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-System

clean-System:
	-$(RM) ./System/User_Init.cyclo ./System/User_Init.d ./System/User_Init.o ./System/User_Init.su ./System/User_MSP2404.cyclo ./System/User_MSP2404.d ./System/User_MSP2404.o ./System/User_MSP2404.su ./System/User_SuperLoop.cyclo ./System/User_SuperLoop.d ./System/User_SuperLoop.o ./System/User_SuperLoop.su ./System/User_TM1638.cyclo ./System/User_TM1638.d ./System/User_TM1638.o ./System/User_TM1638.su

.PHONY: clean-System

