################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/globalData.cpp \
../src/gst_capture.cpp \
../src/main.cpp \
../src/main_cap.cpp \
../src/main_com.cpp \
../src/main_core.cpp \
../src/main_gst.cpp \
../src/main_osd.cpp \
../src/main_process.cpp 

OBJS += \
./src/globalData.o \
./src/gst_capture.o \
./src/main.o \
./src/main_cap.o \
./src/main_com.o \
./src/main_core.o \
./src/main_gst.o \
./src/main_osd.o \
./src/main_process.o 

CPP_DEPS += \
./src/globalData.d \
./src/gst_capture.d \
./src/main.d \
./src/main_cap.d \
./src/main_com.d \
./src/main_core.d \
./src/main_gst.d \
./src/main_osd.d \
./src/main_process.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/GL -I../include -I../src/capture -I../src/encTrans -I../src -I../src/core -I../src/cr_osa/inc -G -g -O0 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_50,code=sm_50 -m64 -odir "src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/GL -I../include -I../src/capture -I../src/encTrans -I../src -I../src/core -I../src/cr_osa/inc -G -g -O0 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


