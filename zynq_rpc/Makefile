PETA_STAGE ?= stage

ifeq ($(wildcard $(PETA_STAGE)),)
$(error Please extract the PetaLinux stage package from CTP7:/mnt/image-persist/linux_stage.tbz2 here to create $(PETA_STAGE)/)
endif

CFLAGS= -fomit-frame-pointer -pipe -fno-common -fno-builtin \
	-Wall \
	-march=armv7-a -mfpu=neon -mfloat-abi=hard \
	-mthumb-interwork -mtune=cortex-a9 \
	-DEMBED -Dlinux -D__linux__ -Dunix -fPIC \
	--sysroot=$(PETA_STAGE) \
	-Irpcsvc_module_dev/

LDLIBS= -L$(PETA_STAGE)/stage/lib \
	-L$(PETA_STAGE)/stage/usr/lib \
	-Lrpcsvc_module_dev/

CXX=arm-linux-gnueabihf-g++

all: CTP7Gen0.so

BUILDINFO_DATA = "Build\t$(shell cat /proc/sys/kernel/random/uuid)\nOn\t$(shell date)\nBy\t$(shell whoami)@$(shell hostname --fqdn)\nFrom\t$(shell git rev-parse --abbrev-ref HEAD)@$(shell git rev-parse HEAD) $(shell git status --porcelain | egrep -q "^[^?]" && echo dirty || echo clean)\n"
export BUILDINFO_DATA

%.so: %.cpp rpcsvc_module_dev/RPCSVC_MANIFEST CTP7Gen0RegMap.hh
	$(CXX) $(CFLAGS) $(LDFLAGS) -fPIC -shared -o $@ $< -DBUILD_INFO="$$BUILDINFO_DATA"

rpcsvc_module_dev/RPCSVC_MANIFEST: module_dev.tbz2
	@rm -rf rpcsvc_module_dev/
	tar -xjf $<
	@touch $@

#LockTools.h LogManager.h moduleapi.h ModuleManager.h wiscRPCMsg.h: rpcsvc_module_dev/RPCSVC_MANIFEST
#	@touch -c rpcsvc_module_dev/$@
#	ln -snf rpcsvc_module_dev/$@ $@

module_dev.tbz2:
	$(error Please place the rpcsvc module_dev.tbz2 package in this directory before running make)

clean:
	rm -rf rpcsvc_module_dev/ *.so *.o

distclean: clean
	rm -rf module_dev.tbz2 stage/ tags

