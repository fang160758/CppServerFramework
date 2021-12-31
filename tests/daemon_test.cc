#include "../inc/daemon.h"
#include "../inc/log.h"
#include "../inc/iomanager.h"


static fang::Logger::ptr g_logger = FANG_LOG_NAME("test");

fang::Timer::ptr timer;

int run(int argc, char** argv) {
    fang::IoManager iom(10);
    timer = iom.addTimer(1000, []() {
            FANG_LOG_INFO(g_logger) << "onTimer";
            static int count = 0;
            if (++count > 100) {
                timer->cancel();
            }
        }, true);
    return 0;

}


int main (int argc, char** argv) {
    return fang::start_daemon(argc, argv, run, argc != 1);
}
