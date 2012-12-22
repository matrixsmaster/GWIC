################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/classes/CGWICDebugUI.cpp \
../src/classes/CGWICGUIObject.cpp \
../src/classes/CGWIC_BodyPart.cpp \
../src/classes/CGWIC_Bot.cpp \
../src/classes/CGWIC_Cell.cpp \
../src/classes/CGWIC_GameObject.cpp \
../src/classes/CGWIC_Head.cpp \
../src/classes/CGWIC_World.cpp \
../src/classes/CPoint2D.cpp 

OBJS += \
./src/classes/CGWICDebugUI.o \
./src/classes/CGWICGUIObject.o \
./src/classes/CGWIC_BodyPart.o \
./src/classes/CGWIC_Bot.o \
./src/classes/CGWIC_Cell.o \
./src/classes/CGWIC_GameObject.o \
./src/classes/CGWIC_Head.o \
./src/classes/CGWIC_World.o \
./src/classes/CPoint2D.o 

CPP_DEPS += \
./src/classes/CGWICDebugUI.d \
./src/classes/CGWICGUIObject.d \
./src/classes/CGWIC_BodyPart.d \
./src/classes/CGWIC_Bot.d \
./src/classes/CGWIC_Cell.d \
./src/classes/CGWIC_GameObject.d \
./src/classes/CGWIC_Head.d \
./src/classes/CGWIC_World.d \
./src/classes/CPoint2D.d 


# Each subdirectory must supply rules for building sources it contributes
src/classes/%.o: ../src/classes/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../src -I../src/classes -I../irrBullet -I/mnt/angar/irrlicht-1.8/include -I/mnt/angar/bullet-2.79/install/include/bullet -I/mnt/angar/OALwrapper/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


