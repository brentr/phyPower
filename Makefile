MODULE := phyPower

obj-m := $(MODULE).o

# build flags
KBUILD_EXTRA_CFLAGS = ""

ifndef KDIR
  ifdef KVER
KDIR := /lib/modules/$(KVER)
  else
KDIR := /lib/modules/$(shell uname -r)
  endif
endif
ifndef KVER
KVER := $(shell basename $(KDIR))
endif

all:
	$(MAKE) -C $(KDIR)/build CFLAGS_MODULE=$(KBUILD_EXTRA_CFLAGS) M=$(shell pwd) modules
clean:
	$(MAKE) -C $(KDIR)/build M=$(shell pwd) clean

install:
	mkdir -p $(KDIR)/updates/
	cp $(MODULE).ko $(KDIR)/updates/
	(cd $(KDIR)/build && /sbin/depmod -ae -F System.map -b modules $(KVER))

