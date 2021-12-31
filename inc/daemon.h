#pragma once

#include <functional>
#include <string>
#include <unistd.h>
#include "singleton.h"




namespace fang {

struct ProcessInfo {

    pid_t parent_id = 0;
    pid_t main_id = 0;
    uint64_t parent_start_time = 0;
    uint64_t main_start_time = 0;
    uint32_t restart_count = 0;

    std::string toString() const;
};

typedef fang::Singleton<ProcessInfo> ProcessInfoMgr;

int start_daemon(int argc, char** argv
            , std::function<int(int argc, char** argv)> main_cb
            , bool is_daemon);
}
