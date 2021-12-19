#ifndef __FANG__MYDEF_H__
#define __FANG__MYDEF_H__
#include <stdbool.h>

#if defined __GNUC__ || defined __llvm__
#define FANG_LIKELY(x)    __builtin_expect(!!(x), 1)
//#define FANG_UNLIKELY(x)  __builtin_expect(!!(x), 0)
#else
#define FANG_LIKELY(x)   (x)
#define FANG_UNLIKELY(x) (x)
#endif





#endif
