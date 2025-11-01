#pragma once 

#define CEIL_DIV(a, b) (((a) + (b) - 1) / (b))
#define FIRST_BIT 1;
#define SECOND_BIT 1 << 1;
#define THIRD_BIT 1 << 2;
#define FOURTH_BIT 1 << 3;
#define FIFTH_BIT 1 << 4;
#define SIXTH_BIT 1 << 5;
#define SEVENTH_BIT 1 << 6;
#define EIGHT_BIT 1 << 7;

typedef char __u8;
typedef unsigned int __u32;
typedef signed int __s32;
typedef unsigned short __u16;
typedef signed short __s16;
typedef unsigned long long __u64;

enum tree_colors { RED, BLACK };