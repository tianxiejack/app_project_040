################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cr_uart/Arachne_IPC_Message.cpp \
../src/cr_uart/Serial_port.cpp \
../src/cr_uart/msgProc.cpp \
../src/cr_uart/uart_api.cpp 

OBJS += \
./src/cr_uart/Arachne_IPC_Message.o \
./src/cr_uart/Serial_port.o \
./src/cr_uart/msgProc.o \
./src/cr_uart/uart_api.o 

CPP_DEPS += \
./src/cr_uart/Arachne_IPC_Message.d \
./src/cr_uart/Serial_port.d \
./src/cr_uart/msgProc.d \
./src/cr_uart/uart_api.d 


# Each subdirectory must supply rules for building sources it contributes
src/cr_uart/%.o: ../src/cr_uart/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/GL -I../include -I../src/capture -I../src/encTrans -I../src -I../src/core -I../include/osa -G -g -O0 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_50,code=sm_50 -m64 -odir "src/cr_uart" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/GL -I../include -I../src/capture -I../src/encTrans -I../src -I../src/core -I../include/osa -G -g -O0 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


