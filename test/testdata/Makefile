# SPDX-FileCopyrightText: 2021 Thomas Mathys
# SPDX-License-Identifier: MIT
#
# To simplify building we have all test data files in version control,
# even binaries for which the source code is included.
# This makefile can be used if test data files need to be (re)created for some reason.

# Nprmally I'm using devkitPro, but any equivalent ARM GNU assembler will do.
AS := arm-none-eabi-as
LD := arm-none-eabi-ld

TARGETS += thumb_entry.bin thumb_entry.elf

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	rm -f $(TARGETS)

%.bin: %.elf
	arm-none-eabi-objcopy -O binary $< $@

%.elf: %.o
	arm-none-eabi-ld -o $@ $<

%.o: %.s
	arm-none-eabi-as -o $@ $<
