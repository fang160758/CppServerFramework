#include "../inc/daemon.h"
#include "../inc/log.h"
#include "../inc/config.h"
#include "../inc/helpc.h"
#include <functional>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>

namespace fang {

static fang::Logger::ptr g_logger = FANG_LOG_NAME("system");
static fang::ConfigVar<uint32_t>::ptr g_daemon_restart_interval
    = fang::Config::Lookup("daemon.restart_interval", (uint32_t)5, "daemon restart interval");



std::string ProcessInfo::toString() const {
    std::stringstream ss;
    ss << "[ProcessInfo parent_id=" << parent_id
       << " main_id=" << main_id
       << " parent_start_time=" << fang::Time2Str(parent_start_time)
       << " main_start_time=" << fang::Time2Str(main_start_time)
       << " restart_count=" << restart_count << "]"
       << std::endl;
    return ss.str();
}


static int real_start(int argc, char** argv
        , std::function<int(int argc, char** argv)> main_cb) {
    ProcessInfoMgr::GetInstance()->main_id = getpid();
    ProcessInfoMgr::GetInstance()->main_start_time = time(0);
    return main_cb(argc, argv);
}


static int real_daemon(int argc, char** argv
        , std::function<int(int argc, char** argv)> main_cb) {
    ProcessInfoMgr::GetInstance()->parent_id = getpid();
    ProcessInfoMgr::GetInstance()->parent_start_time = time(0);

    while(true) {
        pid_t pid = fork();
        if (pid < 0) {
            FANG_LOG_ERROR(g_logger) << "fork fail return=" << pid
                << " errno=" << errno << "errstr=" << strerror(errno);
            return -1;
        } else if (pid == 0) {
            ProcessInfoMgr::GetInstance()->main_id = getpid();
            ProcessInfoMgr::GetInstance()->main_start_time = time(0);
            FANG_LOG_INFO(g_logger) << "process start pid=" << getpid();
            return real_start(argc, argv, main_cb);
        } else {
            int status = 0;
            waitpid(pid, &status, 0);
            if (status) {
                if (status == 9) {
                    FANG_LOG_INFO(g_logger) << "killed";
                    break;
                } else {
                    FANG_LOG_ERROR(g_logger) << "child crash pid=" << pid
                        << " status=" << status;
                }
            } else {
                FANG_LOG_INFO(g_logger) << "child finished pid=" << pid;
                break;
            }
            ProcessInfoMgr::GetInstance()->restart_count += 1;
            sleep(g_daemon_restart_interval->getValue());
        }
    }
    return 0;
}


int start_daemon(int argc, char** argv
        , std::function<int(int argc, char** argv)> main_cb
        , bool is_daemon) {
    if (!is_daemon) {
        ProcessInfoMgr::GetInstance()->parent_id = getpid();
        ProcessInfoMgr::GetInstance()->parent_start_time = time(0);
        return real_start(argc, argv, main_cb);
    }
    return real_daemon(argc, argv, main_cb);
}



}
