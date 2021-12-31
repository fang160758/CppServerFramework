#include "../inc/application.h"
#include "../inc/log.h"
#include "../inc/env.h"
#include "../inc/config.h"
#include "../inc/daemon.h"
#include "../inc/tcp_server.h"
#include "../inc/helpc.h"
#include "../inc/module.h"
#include "../inc/address.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace fang {

static fang::Logger::ptr g_logger = FANG_LOG_NAME("system");


static fang::ConfigVar<std::string>::ptr g_server_work_path
    = fang::Config::Lookup("server.work_path"
            , std::string("/app/work/fang")
            , "server work path");

static fang::ConfigVar<std::string>::ptr g_server_pid_file 
    = fang::Config::Lookup("server.pid_file"
            , std::string("fang.pid")
            , "server pid file");

static fang::ConfigVar<std::vector<TcpServerConf> >::ptr g_servers_conf
    = fang::Config::Lookup("servers"
            , std::vector<TcpServerConf>()
            , "http server config");

static fang::ConfigVar<std::string>::ptr g_service_discovery_zk
    = fang::Config::Lookup("service_discovery.zk"
            , std::string("")
            , "service discovery zookeeper");

Application* Application::s_instance = nullptr;

Application::Application() {
    s_instance = this;
}



bool Application::init(int argc, char** argv) {
    m_argc = argc;
    m_argv = argv;

    fang::EnvMgr::GetInstance()->addHelp("s", "start with the terminal");
    fang::EnvMgr::GetInstance()->addHelp("d", "start with the daemon");
    fang::EnvMgr::GetInstance()->addHelp("c", "conf path default: ./conf");
    fang::EnvMgr::GetInstance()->addHelp("p", "print help");

    bool is_print_help = false;
    if (!fang::EnvMgr::GetInstance()->init(m_argc, m_argv)) {
        is_print_help = true;
    }
    if (fang::EnvMgr::GetInstance()->has("p")) {
        is_print_help = true;
    }

    std::string conf_path = fang::EnvMgr()->GetInstance()->getConfigPath();
    FANG_LOG_INFO(g_logger) << "load conf path:" << conf_path;
    fang::Config::LoadFromConfDir(conf_path);

    ModuleMgr::GetInstance()->init();
    std::vector<Module::ptr> modules;
    ModuleMgr::GetInstance()->listAll(modules);

    for (auto i : modules) {
        i->onBeforeArgsParse(argc, argv);
    }

    if (is_print_help) {
        fang::EnvMgr::GetInstance()->printHelp();
        return false;
    }

    for (auto i : modules) {
        i->onAfterArgsParse(argc, argv);
    }

    module.clear();

    int run_type = 0;
    if (fang::EnvMgr::GetInstance()->has("s")) {
        run_type = 1;
    }
    if (fang::EnvMgr::GetInstance()->has("d")) {
        run_type = 2;
    }

    if (run_type == 0) {
        fang::EnvMgr::GetInstance()->printHelp();
        return false;
    }

    std::string pidfile = g_server_work_path->getValue()
            + "/" + g_server_pid_file->getValue();

    if (fang::Helpc::IsRunningPidfile(pidfile)) {
        FANG_LOG_ERROR(g_logger) << "server is running: " << pidfile;
        return false;
    }

    if (!fang::Helpc::Mkdir(g_server_work_path->getValue())) {
        FANG_LOG_ERROR(g_logger) << "create work path [" << g_server_work_path->getValue()
            << " errno=" << errno << " errstr=" << strerror(errno);
        return false;
    }
    return true;
}


bool Application::run() {
    bool is_daemon = fang::EnvMgr::GetInstance()->has("d");
    return start_daemon(m_argc, m_argv, std::bind(&Application::main, this
                , std::placeholders::_1, std::placeholders::_2), is_daemon);
}

int Application::main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN);
    FANG_LOG_INFO(g_logger) << "Application::main start!";
    std::string conf_path = fang::EnvMgr::GetInstance()->getConfigPath();
    fang::Config::LoadFromConfDir(conf_path, true);
    {
        std::string pidfile = g_server_work_path->getValue()
            + "/" + g_server_pid_file->getValue();
        std::ofstream ofs(pidfile);
        if (!ofs) {
            FANG_LOG_ERROR(g_logger) << "open pidfile " << pidfile << " failed";
            return false;
        }
        ofs << getpid();
    }

    m_mainIOManager.reset(new fang::IoManager(1, true, "main"));
    m_mainIOManager->schedul(std::bind(&Application::run_fiber, this));
    m_mainIOManager->addTimer(2000, [](){
        FANG_LOG_INFO(g_logger) << "hello";
            }, true);
    m_mainIOManager->stop();
    return 0;
}

int Application::run_fiber() {
    std::vector<Module::ptr> modules;
    ModuleMgr::GetInstance()->listAll(modules);

    bool has_error = false;
    for (auto& i : modules) {
        if (!i->onLoad()) {
            FANG_LOG_ERROR(g_logger) << "module name="
                << i->getName() << " version=" << i->getVersion()
                << " filename=" << i->getFilename();
           has_error = true; 
        }
    }
    if (has_error) {
        _exit(0);
    }

    auto http_confs = g_servers_conf->getValue();
    std::vector<TCPServer::ptr> svrs;
    for (auto& i : http_confs) {
        std::vector<Address::ptr> address;
        for (auto& a : i.address) {
            size_t pos = a.find(":");
            if (pos == std::string::npos) {
                address.push_back(UnixAddress::ptr(new UnixAddress(a)));
                continue;
            }

            int32_t port = atoi(a.substr(pos + 1).c_str());

            auto addr = fang::IPAddress::Create(a.substr(0, pos).c_str(), port);
            if (addr) {
                address.push_back(addr);
                continue;
            }
            std::vector<std::pair<Address::ptr, uint32_t> > result;
            if (fang::Address::GetInterfaceAddresses(result, a.substr(0, pos))) {
                for (auto& x : result) {
                    auto ipaddr = std::dynamic_pointer_cast<IPAddress>(x.first);
                    if (ipaddr) {
                        ipaddr->setPort(atoi(a.substr(pos + 1).c_str()));
                    }
                    address.push_back(ipaddr);
                }
                continue;
            }

            auto aaddr = fang::Address::LookupAny(a);
            if (aaddr) {
                address.push_back(aaddr);
                continue;
            }
            FANG_LOG_ERROR(g_logger) << "invalid address= " << a;
            _exit(0);
        }
        IoManager* accept_worker = fang::IoManager::GetThis();
        IoManager* io_worker = fang::IoManager::GetThis();
        IoManager* process_worker = fang::IoManager::GetThis();

        if (!i.accept_worker.empty()) {
            accept_worker = fang::W
        }
    }

}

bool Application::getServer(const std::string& type, std::vector<TCPServer::ptr>& svrs) {
    auto it = m_servers.find(type);
    if (it == m_servers.end()) {
        return false;
    }
    svrs = it->second;
    return true;
}

void Application::listAllServer(std::map<std::string, std::vector<TCPServer::ptr> >& servers) {
    servers = m_servers;
}
}
