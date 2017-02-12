all: targets

BASEDIR=../../
include $(BASEDIR)stpp.mk

LIBS=CherryPickerBoard.o

EXECS:=shell BacAFruitsBT BacAFruits ax12 lcd nfc srf02

ifeq ($(PLAT),stm)
EXECS:=$(addsuffix .flash,$(EXECS)) $(addsuffix .ram,$(EXECS))
endif

$(EXECS): CXXFLAGS+=-I.
$(EXECS): $(LIBS)

targets: $(EXECS)

clean:
	rm -f $(EXECS)
