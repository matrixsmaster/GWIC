################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../irrBullet/boxshape.cpp \
../irrBullet/bulletworld.cpp \
../irrBullet/bvhtrianglemeshshape.cpp \
../irrBullet/collisioncallbackinformation.cpp \
../irrBullet/collisionobject.cpp \
../irrBullet/collisionobjectaffector.cpp \
../irrBullet/collisionobjectaffectorattract.cpp \
../irrBullet/collisionobjectaffectordelete.cpp \
../irrBullet/collisionshape.cpp \
../irrBullet/convexhullshape.cpp \
../irrBullet/gimpactmeshshape.cpp \
../irrBullet/irrbullet.cpp \
../irrBullet/irrbulletcommon.cpp \
../irrBullet/kinematiccharactercontroller.cpp \
../irrBullet/liquidbody.cpp \
../irrBullet/motionstate.cpp \
../irrBullet/physicsdebug.cpp \
../irrBullet/raycastvehicle.cpp \
../irrBullet/rigidbody.cpp \
../irrBullet/softbody.cpp \
../irrBullet/sphereshape.cpp \
../irrBullet/trianglemeshshape.cpp 

OBJS += \
./irrBullet/boxshape.o \
./irrBullet/bulletworld.o \
./irrBullet/bvhtrianglemeshshape.o \
./irrBullet/collisioncallbackinformation.o \
./irrBullet/collisionobject.o \
./irrBullet/collisionobjectaffector.o \
./irrBullet/collisionobjectaffectorattract.o \
./irrBullet/collisionobjectaffectordelete.o \
./irrBullet/collisionshape.o \
./irrBullet/convexhullshape.o \
./irrBullet/gimpactmeshshape.o \
./irrBullet/irrbullet.o \
./irrBullet/irrbulletcommon.o \
./irrBullet/kinematiccharactercontroller.o \
./irrBullet/liquidbody.o \
./irrBullet/motionstate.o \
./irrBullet/physicsdebug.o \
./irrBullet/raycastvehicle.o \
./irrBullet/rigidbody.o \
./irrBullet/softbody.o \
./irrBullet/sphereshape.o \
./irrBullet/trianglemeshshape.o 

CPP_DEPS += \
./irrBullet/boxshape.d \
./irrBullet/bulletworld.d \
./irrBullet/bvhtrianglemeshshape.d \
./irrBullet/collisioncallbackinformation.d \
./irrBullet/collisionobject.d \
./irrBullet/collisionobjectaffector.d \
./irrBullet/collisionobjectaffectorattract.d \
./irrBullet/collisionobjectaffectordelete.d \
./irrBullet/collisionshape.d \
./irrBullet/convexhullshape.d \
./irrBullet/gimpactmeshshape.d \
./irrBullet/irrbullet.d \
./irrBullet/irrbulletcommon.d \
./irrBullet/kinematiccharactercontroller.d \
./irrBullet/liquidbody.d \
./irrBullet/motionstate.d \
./irrBullet/physicsdebug.d \
./irrBullet/raycastvehicle.d \
./irrBullet/rigidbody.d \
./irrBullet/softbody.d \
./irrBullet/sphereshape.d \
./irrBullet/trianglemeshshape.d 


# Each subdirectory must supply rules for building sources it contributes
irrBullet/%.o: ../irrBullet/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../src -I../src/classes -I../irrBullet -I/mnt/angar/irrlicht-1.8/include -I/mnt/angar/bullet-2.79/install/include/bullet -I/mnt/angar/OALwrapper/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


