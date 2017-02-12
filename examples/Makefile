all: targets

BASEDIR=../
include $(BASEDIR)stpp.mk

EXECS=led capa button timer ledstrip lidar motor dumpLcd incrementalEncoder irremote remote rotaryCounter

ifeq ($(PLAT),stm)
EXECS:=$(addsuffix .flash,$(EXECS)) $(addsuffix .ram,$(EXECS))
endif

# Add library dependencies here
# $(EXECS): $(LIBS)

targets: $(EXECS) cherryPicker_dir

cherryPicker_dir:
	$(MAKE) -C cherryPicker all -k

clean:
	rm -f $(EXECS)
