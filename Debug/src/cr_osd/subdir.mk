################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cr_osd/app_proj_xgs040.cpp \
../src/cr_osd/osd_graph.cpp 

OBJS += \
./src/cr_osd/app_proj_xgs040.o \
./src/cr_osd/osd_graph.o 

CPP_DEPS += \
./src/cr_osd/app_proj_xgs040.d \
./src/cr_osd/osd_graph.d 


# Each subdirectory must supply rules for building sources it contributes
src/cr_osd/%.o: ../src/cr_osd/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/GL -I../include -I../src/capture -I../src/encTrans -I../src -I../src/core -I../include/osa -G -g -O0 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_50,code=sm_50 -m64 -odir "src/cr_osd" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/GL -I../include -I../src/capture -I../src/encTrans -I../src -I../src/core -I../include/osa -G -g -O0 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


