#pragma once
#include "stdint.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned __int64 u64;

typedef char i8;

#define U16_MAX UINT16_MAX

#define COPY_FORBID(A)\
A(const A&) = delete; \
A & operator=(const A&) = delete;