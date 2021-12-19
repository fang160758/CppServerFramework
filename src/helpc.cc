/*
 * @Descripttion:
 * @version:
 * @Author: Fang
 * @email: 2192294938@qq.com
 * @Date: 2021-11-23 13:00:28
 * @LastEditors: Fang
 * @LastEditTime: 2021-11-30 10:35:31
 */
#include "helpc.h"
#include <sys/stat.h>
#include <unistd.h>
#include <execinfo.h>
#include "log.h"
namespace fang
{
    fang::Logger::ptr g_logger = FANG_LOG_NAME("system");

    pid_t GetThreadId() { return syscall(SYS_gettid); }
    uint64_t GetFiberId() { return Fiber::GetFiberId(); }
    void BackTrace(std::vector<std::string> &bt, int size, int skip)
    {
        void **array = (void **)malloc(sizeof(void *) * size);
        size_t s = backtrace(array, size);
        char **strings = backtrace_symbols(array, s);
        if (strings == nullptr)
        {
            FANG_LOG_ERROR(g_logger) << "backtrace_symbols error";
            return;
        }
        for (size_t i = skip; i < s; i++)
            bt.push_back(strings[i]);
        free(strings);
        free(array);
    }
    std::string BackTraceToString(int size, int skip, const std::string &prefix)
    {
        std::vector<std::string> bt;
        BackTrace(bt, size, skip);
        std::stringstream t_ss;
        for (unsigned int i = 0; i < bt.size(); i++)
            t_ss << prefix << bt[i] << std::endl;
        return t_ss.str();
    }

    static int __lstat(const std::string &file, struct stat *st = nullptr)
    {
        struct stat lst;
        int ret = lstat(file.c_str(), &lst);
        if (st)
        {
            *st = lst;
        }
        return ret;
    }

    static int __mkdir(const char *dirname)
    {
        if (access(dirname, F_OK) == 0)
        {
            return 0;
        }
        return mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    static bool Mkdir(const std::string &dirname)
    {
        if (__lstat(dirname) == 0)
            return true;
        char *path = strdup(dirname.c_str());
        char *ptr = strchr(path + 1, '/');
        do
        {
            for (; ptr; *ptr = '/', ptr = strchr(ptr + 1, '/'))
            {
                *ptr = '\0';
                if (__mkdir(path) != 0)
                {
                    break;
                }
            }
            if (ptr != nullptr)
            {
                break;
            }
            else if (__mkdir(path) != 0)
            {
                break;
            }
            free(path);
            return true;
        } while (0);
        free(path);
        return false;
    }
    static std::string Dirname(const std::string &filepath)
    {
        if (filepath.empty())
            return ".";
        auto pos = filepath.rfind('/');
        if (pos == 0)
            return "/";
        else if (pos == std::string::npos)
            return ".";
        else
            return filepath.substr(0, pos);
    }

    bool Helpc::OpenFileWirte(std::ofstream &ofs, const std::string &filepath, std::ios_base::openmode mode)
    {
        ofs.open(filepath.c_str(), mode);
        if (!ofs.is_open())
        {
            std::string dir = Dirname(filepath);
            Mkdir(dir);
            ofs.open(filepath.c_str(), mode);
        }
        return ofs.is_open();
    }
}
