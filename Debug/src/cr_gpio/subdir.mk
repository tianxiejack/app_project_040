################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cr_gpio/gpio040.cpp \
../src/cr_gpio/gpio_rdwr.cpp 

OBJS += \
./src/cr_gpio/gpio040.o \
./src/cr_gpio/gpio_rdwr.o 

CPP_DEPS += \
./src/cr_gpio/gpio040.d \
./src/cr_gpio/gpio_rdwr.d 


# Each subdirectory must supply rules for building sources it contributes
src/cr_gpio/%.o: ../src/cr_gpio/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/GL -I../include -I../include/osa -I../src -I../src/capture -O3 -Xcompiler -fopenmp -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_50,code=sm_50 -m64 -odir "src/cr_gpio" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/GL -I../include -I../include/osa -I../src -I../src/capture -O3 -Xcompiler -fopenmp --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


