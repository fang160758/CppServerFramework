#include "../inc/scheduler.h"
#include "../inc/log.h"
#include <unistd.h>

fang::Logger::ptr g_logger = FANG_LOG_ROOT();


void test_fiber() {
    static int count = 5;
    FANG_LOG_INFO(g_logger) << "test in fiber s_count="  << count;

    sleep(1);
    if (--count >= 0) {
        fang::Scheduler::GetThis()->schedul(&test_fiber, fang::GetThreadId());
    }
}




int main()
{
    FANG_LOG_INFO(g_logger) << "main";
    fang::Scheduler sc(3, false, "test");
    sc.start();
    sleep(22);

    FANG_LOG_INFO(g_logger) << "Scheduler";
    sc.schedul(&test_fiber);
    sc.stop();
    return 0;
}
