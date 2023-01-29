<!--
SPDX-FileCopyrightText: 2021 Thomas Mathys
SPDX-License-Identifier: MIT
shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
-->

# GBA cart header
| Adress | Size (bytes) | Can contain code | Description                                        |
|--------|--------------|------------------|----------------------------------------------------|
| 0x00   | 4            | N                | ROM entry point, contains ARM branch instruction   |
| 0x04   | 156          | N                | Nintendo logo                                      |
| 0xa0   | 12           | Y                | Game title, normally uppercase ASCII               |
| 0xac   | 4            | Y                | Game code, normally uppercase ASCII                |
| 0xb0   | 2            | Y                | Maker code, normally uppercase ASCII               |
| 0xb2   | 1            | N (Y[^1])        | Must be 0x96                                       |
| 0xb3   | 1            | Y[^1]            | Main unit code, should be 0 for current GBA models |

[^1]: These two bytes are not particularly useful because of the fixed byte at 0xb2,
      but they can be used to form a Thumb instruction. If no useful instruction can be found,
      a dummy `mov` instruction can be inserted, e.g. `mov r0, 0x96`.
