#ifndef __FANG__MYDEF_H__
#define __FANG__MYDEF_H__
#include <stdbool.h>
#include <assert.h>
#include "log.h"
#if defined __GNUC__ || defined __llvm__
#define FANG_LIKELY(x)    __builtin_expect(!!(x), 1)

#define FANG_UNLIKELY(x)  __builtin_expect(!!(x), 0)
#else
#define FANG_LIKELY(x)   (x)
#define FANG_UNLIKELY(x) (x)
#endif


namespace fang {

#define FANG_ASSERT(x)\
    if (FANG_UNLIKELY(!(x))) {\
        FANG_LOG_ERROR(FANG_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n"\
            << fang::BackTraceToString(100, 2, "    ");\
        assert(x);\
    }

#define FANG_ASSERT2(x, c)\
    if (FANG_UNLIKELY(!(x))) {\
        FANG_LOG_ERROR(FANG_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w\
            << "\nbacktrace:\n"\
            << fang::BackTraceToString(100, 2, "    ");\
        assert(x);\
    }
}




#endif
