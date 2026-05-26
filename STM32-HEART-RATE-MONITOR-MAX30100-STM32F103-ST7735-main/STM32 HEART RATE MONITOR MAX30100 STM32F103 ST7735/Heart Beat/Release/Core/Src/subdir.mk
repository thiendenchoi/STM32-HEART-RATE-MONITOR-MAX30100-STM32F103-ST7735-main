################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/IICcom.c \
../Core/Src/MAX30100.c \
../Core/Src/ST7735.c \
../Core/Src/algorithm.c \
../Core/Src/beep.c \
../Core/Src/blood.c \
../Core/Src/gui.c \
../Core/Src/main.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c \
../Core/Src/user_main.c 

OBJS += \
./Core/Src/IICcom.o \
./Core/Src/MAX30100.o \
./Core/Src/ST7735.o \
./Core/Src/algorithm.o \
./Core/Src/beep.o \
./Core/Src/blood.o \
./Core/Src/gui.o \
./Core/Src/main.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o \
./Core/Src/user_main.o 

C_DEPS += \
./Core/Src/IICcom.d \
./Core/Src/MAX30100.d \
./Core/Src/ST7735.d \
./Core/Src/algorithm.d \
./Core/Src/beep.d \
./Core/Src/blood.d \
./Core/Src/gui.d \
./Core/Src/main.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d \
./Core/Src/user_main.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/IICcom.cyclo ./Core/Src/IICcom.d ./Core/Src/IICcom.o ./Core/Src/IICcom.su ./Core/Src/MAX30100.cyclo ./Core/Src/MAX30100.d ./Core/Src/MAX30100.o ./Core/Src/MAX30100.su ./Core/Src/ST7735.cyclo ./Core/Src/ST7735.d ./Core/Src/ST7735.o ./Core/Src/ST7735.su ./Core/Src/algorithm.cyclo ./Core/Src/algorithm.d ./Core/Src/algorithm.o ./Core/Src/algorithm.su ./Core/Src/beep.cyclo ./Core/Src/beep.d ./Core/Src/beep.o ./Core/Src/beep.su ./Core/Src/blood.cyclo ./Core/Src/blood.d ./Core/Src/blood.o ./Core/Src/blood.su ./Core/Src/gui.cyclo ./Core/Src/gui.d ./Core/Src/gui.o ./Core/Src/gui.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su ./Core/Src/user_main.cyclo ./Core/Src/user_main.d ./Core/Src/user_main.o ./Core/Src/user_main.su

.PHONY: clean-Core-2f-Src

