################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../8led.c \
../button.c \
../cola.c \
../led.c \
../main.c \
../sudoku_2025.c \
../timer.c \
../timer1.c \
../timer2.c \
../timer3.c 

ASM_SRCS += \
../init_b.asm 

OBJS += \
./8led.o \
./button.o \
./cola.o \
./init_b.o \
./led.o \
./main.o \
./sudoku_2025.o \
./timer.o \
./timer1.o \
./timer2.o \
./timer3.o 

C_DEPS += \
./8led.d \
./button.d \
./cola.d \
./led.d \
./main.d \
./sudoku_2025.d \
./timer.d \
./timer1.d \
./timer2.d \
./timer3.d 

ASM_DEPS += \
./init_b.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -I"C:\hlocal\workspace_Hardware\practica2\common" -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mapcs-frame -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC Assembler'
	arm-none-eabi-gcc -x assembler-with-cpp -I"C:\hlocal\workspace_Hardware\practica2\common" -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


