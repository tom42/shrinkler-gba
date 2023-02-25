# Shrinkler Depacker explained
## Contexts and how they are laid out on the stack
The following explanation is from
https://ada.untergrund.net/?p=boardthread&id=264&page=4#msg10080

D6 is usually positive - it is only negative in the single case of
reading the bit indicating whether a reference uses a repeated offset
or not. This bit has a context separate from all the others.
It happens where there is the "moveq.l #-1,d6" instruction.

The instructions calculating the address into the stack
("lea.l 4+SINGLE_BIT_CONTEXTS*2(a7,d6.l),a1" and then "add.l d6,a1")
add 4 to get past the return address of the GetBit call, plus another 2
to make room for that single bit context mentioned earlier.
This way, the address always ends up into the space allocated on the stack.

The upper byte of D6 is set separately from the lower byte. It happens
both in the GetKind and GetNumber procedures, in both cases using the
"lsl.w #8,d6" instruction. The upper byte groups the contexts,
so they are ordered like this:

* -1: Repeated offset (as described above)
* $000: Literal/match select for even bytes
* $0xx: Literal contexts for even bytes
* $100: Literal/match select for odd bytes
* $1xx: Literal contexts for odd bytes
* $3xx: Number contexts for match offsets
* $4xx: Number contexts for match lengths

The reason for skipping $2xx is that the number contexts are accessed
through the GetNumber function, which means there will be one more
return address on the stack once the execution reaches GetBit,
so the effective base address of the context table will be 4 bytes
earlier when accessing the number contexts.
Thus, $2xx would overlap the $1xx contexts slightly.
The easiest (i.e. smallest) way to compensate is simply to not use $2xx.
Also the number contexts don't use the whole ranges
(only up to 30 at max in the lower byte). So it should be possible to
reduce the number of contexts used to less than 600, though that would
make the computation of the context index more involved.
