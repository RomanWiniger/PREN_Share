################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/control/sensor/sensor.c 

C_DEPS += \
./source/control/sensor/sensor.d 

OBJS += \
./source/control/sensor/sensor.o 


# Each subdirectory must supply rules for building sources it contributes
source/control/sensor/%.o: ../source/control/sensor/%.c source/control/sensor/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK22FN512VLL12 -DCPU_MK22FN512VLL12_cm4 -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__NEWLIB__ -I"C:\Users\micro\git\PREN_Share\PREN_Project_Share\source\utils" -I"C:\Users\micro\git\PREN_Share\PREN_Project_Share\source\control\coil" -I"C:\Users\micro\git\PREN_Share\PREN_Project_Share\source\control\motor" -I"C:\Users\micro\git\PREN_Share\PREN_Project_Share\source\control" -I"C:\Users\micro\git\PREN_Share\PREN_Project_Share\source\com" -I"C:\Users\micro\git\PREN_Share\PREN_Project_Share\startup" -I"C:\Users\micro\git\PREN_Share\PREN_Project_Share\source" -I"C:\Users\micro\git\PREN_Share\PREN_Project_Share\drivers" -I"C:\Users\micro\git\PREN_Share\PREN_Project_Share\device" -I"C:\Users\micro\git\PREN_Share\PREN_Project_Share\CMSIS" -I"C:\Users\micro\git\PREN_Share\PREN_Project_Share\board" -I"C:\Users\micro\git\PREN_Share\PREN_Project_Share\source\control\sensor" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -fstack-usage -specs=nano.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-control-2f-sensor

clean-source-2f-control-2f-sensor:
	-$(RM) ./source/control/sensor/sensor.d ./source/control/sensor/sensor.o

.PHONY: clean-source-2f-control-2f-sensor

