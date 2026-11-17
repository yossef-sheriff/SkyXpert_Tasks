################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MCAL/EXTI.c \
../MCAL/GPIO.c \
../MCAL/I2C.c \
../MCAL/SPI.c \
../MCAL/TIMER.c \
../MCAL/UART.c 

OBJS += \
./MCAL/EXTI.o \
./MCAL/GPIO.o \
./MCAL/I2C.o \
./MCAL/SPI.o \
./MCAL/TIMER.o \
./MCAL/UART.o 

C_DEPS += \
./MCAL/EXTI.d \
./MCAL/GPIO.d \
./MCAL/I2C.d \
./MCAL/SPI.d \
./MCAL/TIMER.d \
./MCAL/UART.d 


# Each subdirectory must supply rules for building sources it contributes
MCAL/%.o MCAL/%.su MCAL/%.cyclo: ../MCAL/%.c MCAL/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -c -I../Inc -I"/home/yossef/Zbook/LedBlink/MCAL" -I"/home/yossef/Zbook/LedBlink/HAL" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-MCAL

clean-MCAL:
	-$(RM) ./MCAL/EXTI.cyclo ./MCAL/EXTI.d ./MCAL/EXTI.o ./MCAL/EXTI.su ./MCAL/GPIO.cyclo ./MCAL/GPIO.d ./MCAL/GPIO.o ./MCAL/GPIO.su ./MCAL/I2C.cyclo ./MCAL/I2C.d ./MCAL/I2C.o ./MCAL/I2C.su ./MCAL/SPI.cyclo ./MCAL/SPI.d ./MCAL/SPI.o ./MCAL/SPI.su ./MCAL/TIMER.cyclo ./MCAL/TIMER.d ./MCAL/TIMER.o ./MCAL/TIMER.su ./MCAL/UART.cyclo ./MCAL/UART.d ./MCAL/UART.o ./MCAL/UART.su

.PHONY: clean-MCAL

