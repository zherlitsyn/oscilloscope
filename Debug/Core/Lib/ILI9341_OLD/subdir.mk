################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/ILI9341_OLD/fonts.c \
../Core/Lib/ILI9341_OLD/ili9341.c 

OBJS += \
./Core/Lib/ILI9341_OLD/fonts.o \
./Core/Lib/ILI9341_OLD/ili9341.o 

C_DEPS += \
./Core/Lib/ILI9341_OLD/fonts.d \
./Core/Lib/ILI9341_OLD/ili9341.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/ILI9341_OLD/%.o Core/Lib/ILI9341_OLD/%.su Core/Lib/ILI9341_OLD/%.cyclo: ../Core/Lib/ILI9341_OLD/%.c Core/Lib/ILI9341_OLD/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-ILI9341_OLD

clean-Core-2f-Lib-2f-ILI9341_OLD:
	-$(RM) ./Core/Lib/ILI9341_OLD/fonts.cyclo ./Core/Lib/ILI9341_OLD/fonts.d ./Core/Lib/ILI9341_OLD/fonts.o ./Core/Lib/ILI9341_OLD/fonts.su ./Core/Lib/ILI9341_OLD/ili9341.cyclo ./Core/Lib/ILI9341_OLD/ili9341.d ./Core/Lib/ILI9341_OLD/ili9341.o ./Core/Lib/ILI9341_OLD/ili9341.su

.PHONY: clean-Core-2f-Lib-2f-ILI9341_OLD

