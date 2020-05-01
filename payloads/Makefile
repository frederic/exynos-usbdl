CROSS_COMPILER ?= ~/tools/cross/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
CC      = $(CROSS_COMPILER)gcc
OBJCOPY = $(CROSS_COMPILER)objcopy
CFLAGS  = -nostdlib
LDFLAGS = -Wl,--build-id=none

SRCS := $(wildcard *.S)
PRGS := $(patsubst %.S,%,$(SRCS))
PRG_SUFFIX=.bin
BINS := $(patsubst %,%$(PRG_SUFFIX),$(PRGS))

all: $(BINS)

%.elf: %.S
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.bin: %.elf
	$(OBJCOPY) -O binary -j .text $< $@

.PHONY: clean
clean:
	rm $(BINS)