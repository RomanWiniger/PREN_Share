################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/motor/motor.c 

C_DEPS += \
./source/motor/motor.d 

OBJS += \
./source/motor/motor.o 


# Each subdirectory must supply rules for building sources it contributes
source/motor/%.o: ../source/motor/%.c source/motor/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK22FN512VLL12 -DCPU_MK22FN512VLL12_cm4 -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__NEWLIB__ -I"C:\Users\Pascal\Desktop\HSLU\4_Semester\PREN\Ueb06_Uart\source\utils" -I"C:\Users\Pascal\Desktop\HSLU\4_Semester\PREN\Ueb06_Uart\source\motor" -I"C:\Users\Pascal\Desktop\HSLU\4_Semester\PREN\Ueb06_Uart\source\com" -I"C:\Users\Pascal\Desktop\HSLU\4_Semester\PREN\Ueb06_Uart\startup" -I"C:\Users\Pascal\Desktop\HSLU\4_Semester\PREN\Ueb06_Uart\source" -I"C:\Users\Pascal\Desktop\HSLU\4_Semester\PREN\Ueb06_Uart\drivers" -I"C:\Users\Pascal\Desktop\HSLU\4_Semester\PREN\Ueb06_Uart\device" -I"C:\Users\Pascal\Desktop\HSLU\4_Semester\PREN\Ueb06_Uart\CMSIS" -I"C:\Users\Pascal\Desktop\HSLU\4_Semester\PREN\Ueb06_Uart\board" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -fstack-usage -specs=nano.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-motor

clean-source-2f-motor:
	-$(RM) ./source/motor/motor.d ./source/motor/motor.o

.PHONY: clean-source-2f-motor

