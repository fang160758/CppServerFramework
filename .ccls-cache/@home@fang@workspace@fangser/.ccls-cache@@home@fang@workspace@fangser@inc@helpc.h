/*
 * @Descripttion:
 * @version:
 * @Author: Fang
 * @email: 2192294938@qq.com
 * @Date: 2021-11-23 13:02:02
 * @LastEditors: Fang
 * @LastEditTime: 2021-11-30 10:23:43
 */
#ifndef __FANG_HELPC_H__
#define __FANG_HELPC_H__
#include <fstream>
#include <string.h>
#include <sys/syscall.h>
#include <vector>
#include "fiber.h"
namespace fang
{
    pid_t GetThreadId();
    uint64_t GetFiberId();
    void BackTrace(std::vector<std::string> &bt, int size, int skip = 1);
    std::string BackTraceToString(int size, int skip = 2, const std::string &piexf = "");
    class Helpc
    {
    public:
        static bool OpenFileWirte(std::ofstream &, const std::string &, std::ios_base::openmode);
    };
}

#endif
