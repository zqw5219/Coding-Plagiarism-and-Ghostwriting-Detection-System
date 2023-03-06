/* 
 * CS:APP Data Lab 
 * 
 * <JING TANG jjt5556>
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

//x&0x50505050 would mask all the odd-numbered bits
//and then check the rest of the bits
//if the number after masking equals 0x50505050
//which means all even-numbered bits in word set to 1
int allEvenBits(int x) {
  int a = 0x55;
  a = (a<<8)|0x55;
  a = (a<<8)|0x55;
  a = (a<<8)|0x55;
  return !((a&x)^a);
}


/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */

//by getting the negative number of x and check its sign 
//we could determine whether the number is 0 or not
//but it exclude the first bit
//so I then check the first bit to determine whether it is 0 or not
//if both parts are 0, return 0, which means there's no "1"s at any 
//of the 32 bits

int bang(int x) {
  int a = ~((~x+1)>>31)+1;
  int b = (x>>31)&0x1;
  int c = a|b;
  c = c^0x1;
  return c;
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

//Exclude the situation of +0 and -0
//Exclude the situation that its exponent are all '1's and frac does not equal to zero
//this situation does not exist
//After excluding these two situation, just simply compared the two numbers

int floatIsEqual(unsigned uf, unsigned ug) {
  int a,b,c,d,eight,ff;
  eight = 0x1<<31;
  ff = ~0x0;
  if ((uf == 0x0)|(uf == eight)){
    if((ug == 0x0)|(ug == eight)){
      return 1;
    }
  }
  a = uf<<1;
  a = a>>24;
  b = uf<<9;
  b = b>>9;
  c = ug<<1;
  c = c>>24;
  d = ug<<9;
  d = ug>>9;
  if((a==ff)&& (b!=0)){
    if((c==ff)&&(d!=0)){
      return 0;
    }  	
  } 
  if (uf == ug){
     return 1;
  }
  else{
    return 0;
  } 
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

//Frist, find the first '1' in order to determine how much should E be
//if E is larger than 23, push the number to the right
//by comparing the digits which are overflowed to 1/2 to determine how to round the rest of the number
//if E is smaller than 23, mask the first '1' and push it to the left
//add bias,which is 127 in this case to E to find the exponent and push it 23 digits left
//use or to combine the exponent and fraction part

unsigned floatUnsigned2Float(unsigned u) {
  int one,bits,s,round,frac,result,mask,compare,pb,pd;
  one = 0x1<<31;
  s=1;
  if(u==0){
    return 0;
  }
  bits = 31;
  while ((u &(0x1<<bits))==0){
    bits = bits -1;
  }
  if(bits > 23){
    s = bits - 23;
    frac = u>>s;
    mask = one >>(31-s);
    mask = ~mask;
    round = u&mask;
  } 
  else{
    frac = u<<(23-bits);
    round = 0;
  }
  mask = one >> 8;
  mask = ~mask;
  frac = frac&mask;
  result = ((bits+127)<<23)|frac; 
  compare = 0x1<<(s-1);
  pb = 0x1<<s;
  pd = u&pb;
  if(round > compare){
    result = result+1;
  }
  if(round == compare){
    if(pd !=0 ){
      result = result+1; 
    }
  } 
  return result;
}
/* 
 * isLess - if x < y  then return 1, else return 0 
 *   Example: isLess(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */

//first determine whether x and y have the same sign or not
//if they have different sign, than only see the sign of x
//if x is negative, x<if x is possitive, x>y
//if they have the same sign, then x-y must not be overflow
//then check the sign of x-y
//if x-y<0, x<y
//if x-y>0, x>y

int isLess(int x, int y) {
  int a,c1,c2,c,y1;
  int sign =(x^y)>>31;
  sign = ~sign;
  y1 = ~y+1;
  a = x+y1;
  a = !!(a>>31);
  c1 = sign&a;
  c2 = (!sign)&(x>>31);
  c = c1|c2;
  return c;
}
/* 
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */

//shift x 31 bits left would determine whether it is negative or not
//if it is negative, return 0xffffffff;
//if it is positive, return 0x00000000;
//then add 1 to it, negative number would return 0
//and nonnagative number would return 1

int isNonNegative(int x) {
  int a = x >> 31;
  return a+1;
}
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */

//only 0x7fffffff and 0xffffffff satisfy the condition that x+1 = ~x
//then exclude the situation of 0xffffffff
int isTmax(int x) {
  int a,b,c,d,f,g,h;
  a = ~x;
  b = x+1;
  c =!(a^b);
  d = ~0x0;
  f = x^d;
  g =!!f;
  h = c&g;
  return h;
}
/* 
 * rotateLeft - Rotate x to the left by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateLeft(0x87654321,4) = 0x76543218
 *   Legal ops: ~ & ^ | + << >> !
 *   Max ops: 25
 *   Rating: 3 
 */
int rotateLeft(int x, int n) {
  int a,b,c,d,e,bit;
  bit =32+(~n+1);
  a = x>>bit;
  b = 0x1<<31;
  e = ~0x0;
  b = b>>(bit+e);
  b = ~b;
  a = a&b;
  c = x<<n;
  d = a|c;
  return d;
}
/* 
 * signMag2TwosComp - Convert from sign-magnitude to two's complement
 *   where the MSB is the sign bit
 *   Example: signMag2TwosComp(0x80000005) = -5.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */

//x&80000000 remains only the sign bit
//use it to minus x, for negative number, it would directly get the 2's complement 
//c&a would remain the result of negative number and turn the result of postive number to 0
//which would not affect the latter transition for positive number
//for positive number, I use ~a&x to copy the number
//and meanwhile, it would not affect the result of negative number

int signMag2TwosComp(int x) {
  int a,b,c,d;
  a = x >> 31;
  b = ~a&x;
  c = 0x1<<31;
  c = (x&c)+(~x+1);
  c = c&a;
  d = c|b;
  return d;
}
/* 
 * thirdBits - return word with every third bit (starting from the LSB) set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */

//for this case, the result is a paticular number, which is 0x49249249
//use shift to build the number
int thirdBits(void) {
  int a = 0x49;
  a = (a<<8)|0x24;
  a = (a<<8)|0x92;
  a = (a<<8)|0x49;
  return a;
}
