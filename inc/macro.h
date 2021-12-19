/*
 * @Descripttion:
 * @version:
 * @Author: Fang
 * @email: 2192294938@qq.com
 * @Date: 2021-11-29 21:11:28
 * @LastEditors: Fang
 * @LastEditTime: 2021-11-29 23:40:33
 */
#ifndef __FANG_MACRO_H__
#define __FANG_MACRO_H__
#include <string.h>
#include <assert.h>
#include "log.h"

#define FANG_ASSERT(X)                                  \
    if (!(X))                                           \
    {                                                   \
        FANG_LOG_ERROR(FANG_LOG_ROOT())                 \
            << "ASSERTION: " << #X                      \
            << "\nbacttrace:\n"                         \
            << fang::BackTraceToString(100, 2, "    "); \
        assert(X);                                      \
    }

#define FANG_ASSERT2(X, W)                              \
    if (!(X))                                           \
    {                                                   \
        FANG_LOG_ERROR(FANG_LOG_ROOT())                 \
            << "ASSERTION: " << #X                      \
            << "\n"                                     \
            << (W)                                      \
            << "\nbacttrace:\n"                         \
            << fang::BackTraceToString(100, 2, "    "); \
        assert(X);                                      \
    }

#endif
