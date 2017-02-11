BASEDIR?=./

# STPP library common elements
# Include this file in your makefile to pull the definitions needed to build stpp applications
STM32_INC=-I$(BASEDIR)3rdparty/CMSIS/Include/
ADAFRUIT_INC=-I$(BASEDIR)3rdparty/Adafruit-GFX-Library/
FREERTOS_BASE=$(BASEDIR)3rdparty/FreeRTOS/Source
FREERTOS_INC=-I$(FREERTOS_BASE)/include/ -I$(FREERTOS_BASE)/portable/GCC/ARM_CM4F/

HOST_ARCH=$(shell arch)
PLAT?=stm
ifeq ($(PLAT),stm)
include $(BASEDIR)usb.mk
ARCH_CFLAGS=-DARCH=stm -fshort-double
ifeq ($(HOST_ARCH),armv7l)
	PREFIX=
endif
PREFIX?=arm-none-eabi-
CXXFLAGS=-mcpu=cortex-m4 -g -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
LDFLAGS=
else
ARCH_CFLAGS=-DARCH=unix
CXXFLAGS=-g
FREERTOS_OBJS=
LDFLAGS=-lpthread
endif

ASFLAGS:=$(CXXFLAGS)
#LDLIBS:=$(shell $(PREFIX)gcc -print-libgcc-file-name)
CXXFLAGS+=-I$(BASEDIR)inc -I$(BASEDIR)shell/inc -I$(BASEDIR)drivers/inc -I$(BASEDIR)plat-inc $(FREERTOS_INC) $(STM32_INC) $(USB_INC) $(ADAFRUIT_INC) -Wall -fno-stack-protector -Os -g3 -DARM_MATH_CM4 -D__FPU_PRESENT=1 $(ARCH_CFLAGS) -fdata-sections -ffunction-sections -Werror -Wextra
CXXFLAGS+=-Wno-deprecated-declarations
CFLAGS:=$(CXXFLAGS) -std=c11
CXXFLAGS+=-fno-rtti -fno-exceptions -std=c++11 -fno-threadsafe-statics

CC=$(PREFIX)gcc
CXX=$(PREFIX)gcc
LD=$(PREFIX)ld
AS=$(PREFIX)as

STPP_LIB = $(BASEDIR)libSTpp.a

# These meta targets are used to build actual STM32 binaries
%.ram: %.o $(STPP_LIB)
	$(LD) $^ -o $@ $(LDFLAGS) -T$(BASEDIR)src/ram.lds $(LDLIBS)

%.flash: %.o $(STPP_LIB)
	$(LD) $^ -o $@ $(LDFLAGS) -T$(BASEDIR)src/flash.lds $(LDLIBS)
