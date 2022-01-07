# Build using devkitPro:
#
# arm-none-eabi-as thumb_entry.s -o thumb_entry.o
# arm-none-eabi-ld thumb_entry.o -o thumb_entry.elf
#
                .cpu    arm7tdmi
                .text
                .global _start
                .thumb
                .thumb_func
_start:
                b .
