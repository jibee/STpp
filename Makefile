# STpp library main build file
# Will build the STpp library and examples too
include stpp.mk

FREERTOS_SRCS+=$(FREERTOS_BASE)/tasks.c
FREERTOS_SRCS+=$(FREERTOS_BASE)/timers.c
FREERTOS_SRCS+=$(FREERTOS_BASE)/queue.c
FREERTOS_SRCS+=$(FREERTOS_BASE)/portable/MemMang/heap_1.c
FREERTOS_SRCS+=$(FREERTOS_BASE)/list.c
FREERTOS_SRCS+=$(FREERTOS_BASE)/portable/GCC/ARM_CM4F/port.c
FREERTOS_OBJS=$(subst .c,.o,$(FREERTOS_SRCS))


SRC:=$(wildcard src/*.c) $(wildcard src/*.s)
SRC_OBJS:=$(subst .c,.o,$(SRC))
SRC_OBJS:=$(subst .s,.o,$(SRC_OBJS))

GFX_SRC = $(wildcard 3rdparty/Adafruit-GFX-Library/*.cpp)
GFX_OBJS=$(subst cpp,o,$(GFX_SRC))

LIB_SRC=$(wildcard lib/*.cpp) $(wildcard lib/*.c) $(wildcard lib/AdaFruit/*.cpp) $(GFX_SRC)
LIB_SRC+=AnatoleBox/Box.cpp AnatoleBox/RGBLedArray.cpp AnatoleBox/AnatoleBox.cpp

LIB_OBJS=$(subst cpp,o,$(LIB_SRC)) lib/debug.o
LIB_OBJS:=$(subst .c,.o,$(LIB_OBJS))
LIB_INCS=$(wildcard inc/*.h)

DRIVERS_SRC=$(wildcard drivers/*.cpp) $(wildcard drivers/*.c)
DRIVERS_OBJS=$(subst .cpp,.o,$(DRIVERS_SRC))
DRIVERS_OBJS:=$(subst .c,.o,$(DRIVERS_OBJS))
DRIVERS_INCS=$(wildcard drivers/inc/*.h)

PLAT_SRC=$(wildcard plat-$(PLAT)/*.cpp)
PLAT_OBJS=$(subst cpp,o,$(PLAT_SRC))
PLAT_INCS=$(wildcard plat-inc/*.h)


BAD_FILES:=3rdparty/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver//src/usb_core.o lib/arm_sin_cos_f32.o
BAD_FILES+=3rdparty/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver//src/usb_dcd_int.o
BAD_FILES+=3rdparty/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library//Core/src/usbd_core.o
BAD_FILES+=3rdparty/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library//Core/src/usbd_req.o
BAD_FILES+=3rdparty/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library//Class/hid/src/usbd_hid_core.o
BAD_FILES+=3rdparty/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library//Class/cdc/src/usbd_cdc_core.o
BAD_FILES+=3rdparty/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_HOST_Library//Core/src/usbh_core.o
BAD_FILES+=3rdparty/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_HOST_Library//Core/src/usbh_hcs.o
BAD_FILES+=3rdparty/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_HOST_Library//Core/src/usbh_ioreq.o
$(BAD_FILES): CFLAGS+=-Wno-sign-compare -Wno-strict-aliasing -Wno-unused-parameter
$(GFX_OBJS): CXXFLAGS+=-Wno-sign-compare -Wno-strict-aliasing -Wno-unused-parameter -Wno-unused-variable

OBJS=$(PLAT_OBJS) $(FREERTOS_OBJS) $(USB_OBJS) $(DRIVERS_OBJS) $(SHELL_OBJS) $(LIB_OBJS) $(SRC_OBJS)

lib/%.o: lib/%.cpp $(LIB_INCS) $(PLAT_INCS) $(DRIVER_INCS)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

drivers/%.o: drivers/%.cpp $(DRIVERS_INCS) $(PLAT_INCS)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

ifeq ($(PLAT),stm)
libSTpp.a: $(OBJS)
	ar rcs $@ $^
else
examples/%: examples/%.o $(OBJS)
	g++ $^ -o $@ $(LDFLAGS)
endif


TARGETS:=led capa button timer ledstrip lidar motor dumpLcd incrementalEncoder irremote remote rotaryCounter
CHERRY_PICKER_TARGETS:=cherryPicker/shell cherryPicker/BacAFruitsBT cherryPicker/BacAFruits cherryPicker/ax12 cherryPicker/lcd cherryPicker/nfc cherryPicker/srf02
EXECS:=$(addprefix examples/,$(TARGETS))
EXECS+=$(ANATOLEBOX_TARGETS)
ifeq ($(PLAT),stm)
EXECS:=$(addsuffix .flash,$(EXECS)) $(addsuffix .ram,$(EXECS))
endif

all: $(EXECS) 

doc:
	doxygen

.SECONDARY: $(LIB_OBJS) $(FREERTOS_OBJS) $(SRC_OBJS) $(USB_OBJS) $(PLAT_OBJS) $(DRIVERS_OBJS) $(EXECS)

CHERRYPICKER_LIB = examples/cherryPicker/CherryPickerBoard.o

$(CHERRY_PICKER_TARGETS): LDLIBS+=$(CHERRY_PICKER_TARGETS)

clean:
	-rm -f examples/*.flash examples/*.ram examples/cherryPicker/*.flash examples/cherryPicker/*.ram libSTpp.a $(FREERTOS_OBJS) $(LIB_OBJS) $(SRC_OBJS) $(USB_OBJS) $(PLAT_OBJS) $(DRIVERS_OBJS) $(SHELL_OBJS)
