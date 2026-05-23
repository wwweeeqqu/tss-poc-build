# Minimal LKM Makefile.
# Run on Linux box (WSL Ubuntu or native) with matching kernel-headers.
#
# Target: ARM64 (Android device), built either via:
#   (a) Native ARM64 Linux (rare) — just `make`
#   (b) x86_64 Linux cross-compiling with ARM64 kernel headers (typical)
#
# For Android target with kernel 6.1.x:
#   1. Either install matching `linux-headers-6.1.x` (Debian/Ubuntu) on
#      an ARM64 Linux system; OR
#   2. Clone Linux 6.1.x source, cross-build with Android NDK / clang.
#
# After build, modinfo tss_poc.ko shows vermagic. If vermagic doesn't match
# the target device, use ../patch_vermagic.py to overwrite it.

obj-m := tss_poc.o tss_dump_min.o

# Default: build against currently running kernel's headers.
# Override with KDIR=/path/to/kernel/build for cross builds.
KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Android cross-compile (set in env or pass on command line):
#   ARCH=arm64 CROSS_COMPILE=aarch64-linux-android- LLVM=1 make
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

modinfo:
	modinfo tss_poc.ko 2>&1 | head -20

.PHONY: all clean modinfo
