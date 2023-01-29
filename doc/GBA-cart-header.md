<!--
SPDX-FileCopyrightText: 2021 Thomas Mathys
SPDX-License-Identifier: MIT
shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
-->

# GBA cart header

The following table shows the fields of the GBA cart header and whether they can be abused to contain code.

| Adress | Size (bytes) | Can contain code | Description                                        |
|--------|--------------|------------------|----------------------------------------------------|
| 0x00   | 4            | N                | ROM entry point, contains ARM branch instruction   |
| 0x04   | 156          | N                | Nintendo logo                                      |
| 0xa0   | 12           | Y                | Game title, normally uppercase ASCII               |
| 0xac   | 4            | Y                | Game code, normally uppercase ASCII                |
| 0xb0   | 2            | Y                | Maker code, normally uppercase ASCII               |
| 0xb2   | 1            | N[^1]            | Must be 0x96                                       |
| 0xb3   | 1            | Y[^1]            | Main unit code, should be 0 for current GBA models |
| 0xb4   | 1            | Y                | Device type, normally 0[^2]                        |
| 0xb5   | 7            | Y                | Reserved, normally 0                               |
| 0xbc   | 1            | Y[^3]            | Game version, normally 0                           |
| 0xbd   | 1            | N                | Complement (header checksum)                       |
| 0xbe   | 2            | Y                | Reserved, normally 0                               |

[^1]: These two bytes are not particularly useful because of the fixed byte at 0xb2,
      but they can be used to form a Thumb instruction. If no useful instruction can be found,
      a dummy `mov` instruction can be inserted, e.g. `mov r0, 0x96`.
[^2]: Bit 7 has a meaning for Nintendo's hardware debugger.
      It seems to have no meaning for normal cartridges.
[^3]: Can be freely chosen, but is hard to make use of since it's followed by the complement.
