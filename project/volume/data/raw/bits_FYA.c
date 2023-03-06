/* 
 * CS:APP Data Lab 
 * 
 * Yuan Fu
 * ykf5041@psu.edu
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2012 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* We do support the IEC 559 math functionality, real and complex.  */
/* wchar_t uses ISO/IEC 10646 (2nd ed., published 2011-03-15) /
   Unicode 6.0.  */
/* We do not support C11 <threads.h>.  */
/* 
 * allEvenBits - return 1 if all even-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allEvenBits(0xFFFFFFFE) = 0, allEvenBits(0x55555555) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */

/*
 * align and AND every two bits, four bits, ...
 * finally every even bits to the LSB.
 * Then AND with 1 to return either 0 or 1.
 */
int allEvenBits(int x) {
  x &= (x>>2);
  x &= (x>>4);
  x &= (x>>8);
  x &= (x>>16);
  return x & 1;
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */

/*
 * Return 0 if any bit of x has 1 in it, otherwise 1.
 * Align and OR every bit to LSB and reverse it.
 * Then and with 1 to return either 0 or 1.
 */
int bang(int x) {
  x |= (x>>1);
  x |= (x>>2);
  x |= (x>>4);
  x |= (x>>8);
  x |= (x>>16);
  return (~x) & 1;
}
/* 
 * floatIsEqual - Compute f == g for floating point arguments f and g.
 *   Both the arguments are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   If either argument is NaN, return 0.
 *   +0 and -0 are considered equal.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 25
 *   Rating: 2
 */

/*
 * First check if uf = ug = 0, if so return 1.
 * Then check if either uf or ug is NaN (exp is FFFF),
 * if so return 0.
 * Then compare each bit with XOR, if any bit is not equal,
 * return 0, else return 1.
 */
int floatIsEqual(unsigned uf, unsigned ug) {
  // both zero
  if (!(((uf << 1) >> 22) | (uf & 0x007FFFFF)) &
      !(((ug << 1) >> 22) | (ug & 0x007FFFFF)))
    return 1;
  // NaN
  if ((uf >= 0x7F800001 && uf <= 0x7FFFFFFF) ||
      (uf >= 0xFF800001) ||
      (ug >= 0x7F800001 && ug <= 0x7FFFFFFF) ||
      (ug >= 0xFF800001))
    return 0;
  // not bit equivalent
  if (uf ^ ug)
    return 0;
  // else
  return 1;
}
/* 
 * floatUnsigned2Float - Return bit-level equivalent of expression (float) u
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */

/*
 * First we get the mantissa part. Keep shifting right until we see the first 1.
 * The number of bits we shifted becomes exponent, add 127 bias to it (we do this at the end).
 * If shifted 32 times and didn’t find 1, then it is 0, return 0
 * Then, count 24 bits after that bit,
 * the first bit to the 24th bit after it is the mantissa.
 * If The rest of the unsigned number is longer than 24 bits (including the normalized bit),
 * we might need rounding.
 * We set roundbit to the bit after the last bit of mantissa.
 * If its 1, round up, if its 0, round down.
 * However, if the roundbit is 1 and the rest of the bits are all 0, or it is the last bit,
 * we need to round to even. In that case set roundbit to 1 if the last bit
 * of mantissa is 1 and set to 0 if the last bit is 0.
 * Finally we get mantissa by shifting the mantissa portion to the end and add roundbit.
 * If adding roundbit causes overflow, set mantissa to 0, and add one to exponent.
 * Shift exponent to the proper position and OR exponent and mantissa together
 * Sign bit is always 0.
 */
unsigned floatUnsigned2Float(unsigned u) {
  // idx for first 1
  int f1idx = 0;
  int af1idx;
  unsigned int roundbit = 0;
  int round2even;
  unsigned int mag;
  unsigned int exp;
  int shiftamt;
  // logical shift
  while ((((u << f1idx) >> 31) != 1) && f1idx <= 31)
    f1idx++;
  if (f1idx == 32) {
    // u is zero
    return 0;
  } else {
    // u is not zero
    af1idx = f1idx + 1;
    if (9 > af1idx) {
      // needs rounding
      roundbit = (u << (af1idx + 23)) >> 31;
      shiftamt = (af1idx + 24);
      if (shiftamt == 32) {
        round2even = 1;
      } else {
        round2even = (roundbit && (u << shiftamt) == 0);
      }
      // round to even
      if (round2even && ((u << (af1idx + 22)) >> 31) == 0)
        roundbit = 0;
    }
    mag = ((u << af1idx) >> 9) + roundbit;
    // 31 - x + 127
    exp = (158 - f1idx);
    // if adding round bit overflows
    if (mag >> 23) {
      mag = 0;
      exp = exp + 1;
    }
    exp = exp << 23;
    return mag | exp;
  }
}
/* 
 * isLess - if x < y  then return 1, else return 0 
 *   Example: isLess(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */

/*
 * If xy have different sign, we compute x & ~y
 * which is 1 if x neg y pos and 0 if x pos y neg in the first bit.
 * If they have the same sign, we compute ~(y - x) and look at the first
 * bit of the result. It is 1 if x <= y and 0 if x > y.
 * We mask both results with sign_mask with only leaves the first
 * bit, OR them together. !! converts bits into 1 or 0.
 * Finally we AND with (x != y), so that if x = y we return 0.
 */
int isLess(int x, int y) {
  int xneg_ypos = x & ~y;
  int first_bit_1_xy_same_sign = ~(x ^ y);
  int sign_mask = 1 << 31;
  int rest_mask = ~sign_mask;
  int ymag = rest_mask & y;
  int xmag = rest_mask & x;
  int yminusx_pos0 = (ymag + (~xmag + 1));
  int xneqy1 = !!(x ^ y);
  return (!!((xneg_ypos | (first_bit_1_xy_same_sign & ~yminusx_pos0)) & sign_mask)) & xneqy1;

}
/* 
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */

/*
 * Reverse x so the sign bit is reversed (1 for non-neg, 0 for neg), then shift it to LSB.
 * Now we have either FFFFFFFF or 00000000.
 * Apply two ! to have 0 or 1 but don’t change LSB.
 */
int isNonNegative(int x) {
  return !(!(~x >> 31));
}
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */

/*
 * We check if MSB is 0 and all other bits are 1.
 * !!~x gives 1 if MSB = 0 and 0 otherwise.
 * !(~x + ~x) if all other bis are one, adding the reverse to reverse
 * should give all 0, ! to it to get 1, if not all 1 the result is 0.
 * AND this two parts to get final result.
 */
int isTmax(int x) {
  return !!~x & !(~x + ~x);
}
/* 
 * rotateLeft - Rotate x to the left by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateLeft(0x87654321,4) = 0x76543218
 *   Legal ops: ~ & ^ | + << >> !
 *   Max ops: 25
 *   Rating: 3 
 */

/*
 * We shift every bit n bits left, and move the thrown-away bits
 * to the right and OR then back together.
 * Move to right is done by shifting right (32 - n) bits.
 * And since we can’t use unsigned and don’t have logical shift,
 * AND with 00...00011...111 mask where 0 part is the shift left part
 * and 1 part is the shift right part.
 * Finally we can OR the two parts together.
 */
int rotateLeft(int x, int n) {
  return (x << n) | ((x >> (32 + (~n + 1))) & ~(~0 << n));
}
/* 
 * signMag2TwosComp - Convert from sign-magnitude to two's complement
 *   where the MSB is the sign bit
 *   Example: signMag2TwosComp(0x80000005) = -5.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */

/*
 * we first get the sign, ~(x >> 31) gives
 * all 1 if positive, all 0 otherwise
 * Then we get the magnitude by masking 7FFFFFFF with x.
 * We calculate the 2’complement version of negative of magnitude
 * We use the sign as a mask, so if positive the positive magnitude is
 * returned, otherwise negative version is returned.
 */
int signMag2TwosComp(int x) {
  int pos1 = ~(x >> 31);
  int mag = ~(1 << 31) & x;
  int neg2c = ~mag + 1;
  return (pos1 & mag) | (~pos1 & neg2c);
}
/* 
 * thirdBits - return word with every third bit (starting from the LSB) set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */

/*
 * We shift 001001 (9) left 6,12,24 bits
 * and OR then together to get the right 24 bits.
 * The rest 8 bits is 01001001 (73), shift it right 24 bits
 * and OR with the first part.
 */
int thirdBits(void) {
  int x = 9 | (9 << 6);
  x |= (x << 12);
  x |= (x << 24);
  return x | (73 << 24);
}
