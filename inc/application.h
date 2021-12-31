#pragma once 
#include "iomanager.h"
#include "tcp_server.h"
#include <sstream>
#include <string>
namespace fang {
class Application {
public:
    Application();
    static Application* GetInstance() { return s_instance; }

    bool init(int argc, char** argv);
    bool run();

    bool getServer(const std::string& type, std::vector<TCPServer::ptr>& svrs);
    void listAllServer(std::map<std::string, std::vector<TCPServer::ptr> >& servers);


private:
    int main(int argc, char** argv);
    int run_fiber();

private:
    int m_argc = 0;
    char** m_argv = nullptr;
    
    IoManager::ptr m_mainIOManager;
    static Application* s_instance;

    std::map<std::string, std::vector<TCPServer::ptr> > m_servers;
    

};

}
