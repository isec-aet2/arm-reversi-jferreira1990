################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/ft6x06/ft6x06.c 

OBJS += \
./Drivers/BSP/Components/ft6x06/ft6x06.o 

C_DEPS += \
./Drivers/BSP/Components/ft6x06/ft6x06.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/ft6x06/ft6x06.o: ../Drivers/BSP/Components/ft6x06/ft6x06.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F769xx -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I"C:/Users/HP/STM32Cube/Repository/STM32Cube_FW_F7_V1.15.0/Drivers/BSP/STM32F769I-Discovery" -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/Components/ft6x06/ft6x06.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

