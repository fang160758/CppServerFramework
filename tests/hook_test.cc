#include "../inc/hook.h"

void test_sleep() {
    sleep(10);
}


int main()
{
    fang::set_hook_enable(true);
    test_sleep();
}
