#include "../inc/env.h"
#include <iostream>
#include <iterator>


int main(int argc, char** argv) {
    fang::EnvMgr::GetInstance()->addHelp("s", "start with the terminal");
    fang::EnvMgr::GetInstance()->addHelp("p", "print help");
    fang::EnvMgr::GetInstance()->addHelp("d", "start with the daemon");
    if (!fang::EnvMgr::GetInstance()->init(argc, argv)) {
        fang::EnvMgr::GetInstance()->printHelp();
        return 0;
    }

    std::cout << "exe=" << fang::EnvMgr::GetInstance()->getExe() << std::endl;
    std::cout << "cwd=" << fang::EnvMgr::GetInstance()->getCwd() << std::endl;
    std::cout << "path=" << fang::EnvMgr::GetInstance()->getEnv("PATH", "xxxx") << std::endl;
    fang::EnvMgr::GetInstance()->printHelp(); 
}
