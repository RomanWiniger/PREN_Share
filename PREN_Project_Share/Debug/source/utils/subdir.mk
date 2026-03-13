################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/utils/debug.c \
../source/utils/ftm0.c \
../source/utils/util.c \
../source/utils/wait.c 

C_DEPS += \
./source/utils/debug.d \
./source/utils/ftm0.d \
./source/utils/util.d \
./source/utils/wait.d 

OBJS += \
./source/utils/debug.o \
./source/utils/ftm0.o \
./source/utils/util.o \
./source/utils/wait.o 


# Each subdirectory must supply rules for building sources it contributes
source/utils/%.o: ../source/utils/%.c source/utils/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK22FN512VLL12 -DCPU_MK22FN512VLL12_cm4 -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__NEWLIB__ -I"C:\Users\roman\myGitRepo\PREN_Project\source\utils" -I"C:\Users\roman\myGitRepo\PREN_Project\source\control\motor" -I"C:\Users\roman\myGitRepo\PREN_Project\source\control" -I"C:\Users\roman\myGitRepo\PREN_Project\source\com" -I"C:\Users\roman\myGitRepo\PREN_Project\startup" -I"C:\Users\roman\myGitRepo\PREN_Project\source" -I"C:\Users\roman\myGitRepo\PREN_Project\drivers" -I"C:\Users\roman\myGitRepo\PREN_Project\device" -I"C:\Users\roman\myGitRepo\PREN_Project\CMSIS" -I"C:\Users\roman\myGitRepo\PREN_Project\board" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -fstack-usage -specs=nano.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-utils

clean-source-2f-utils:
	-$(RM) ./source/utils/debug.d ./source/utils/debug.o ./source/utils/ftm0.d ./source/utils/ftm0.o ./source/utils/util.d ./source/utils/util.o ./source/utils/wait.d ./source/utils/wait.o

.PHONY: clean-source-2f-utils

