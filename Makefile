obj-m := tss_poc.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

ARCH ?=
CROSS_COMPILE ?=
LLVM ?=

ifneq ($(CROSS_COMPILE),)
EXTRA_FLAGS := ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) LLVM=$(LLVM)
endif

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules $(EXTRA_FLAGS)

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

.PHONY: all clean
