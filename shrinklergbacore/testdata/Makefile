# SPDX-FileCopyrightText: 2021 Thomas Mathys
# SPDX-License-Identifier: MIT
# shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

# Using devkitPro, but any ARM GNU assembler should do.
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
