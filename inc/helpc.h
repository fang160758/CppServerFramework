#ifndef __FANG_HELPC_H__
#define __FANG_HELPC_H__

#include <fstream>
#include <string.h>
#include <string>
#include <sys/syscall.h>
#include <vector>
#include "fiber.h"
#include <cxxabi.h>

namespace fang
{
    pid_t GetThreadId();
    uint64_t GetFiberId();
    uint64_t GetCurrentMS();
    void BackTrace(std::vector<std::string> &bt, int size, int skip = 1);
    std::string BackTraceToString(int size, int skip = 2, const std::string &piexf = "");
    class Helpc
    {
    public:
        static bool Mkdir(const std::string &dirname);
        static bool OpenFileWirte(std::ofstream &, const std::string &, std::ios_base::openmode);
        static void ListAllFile(std::vector<std::string>& files
                                , const std::string& path
                                , const std::string& subfix);

        static bool IsRunningPidfile(const std::string& pidfile);
        static bool Unlink(const std::string& filename, bool exist = false);
    };

    template<typename T>
    const char* TypeToName() {
        static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
        return s_name;
    }

    std::string Time2Str(time_t ts = time(0), const std::string& format = "%Y-%m-%d %H:%M:%S");
    //time_t Str2Time(const char* str, const char* format);
}

#endif
