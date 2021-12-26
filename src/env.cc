#include "../inc/env.h"
#include "../inc/log.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>

namespace fang {
static fang::Logger::ptr g_logger = FANG_LOG_NAME("system");

bool Env::init(int argc, char** argv) {
    char link[1024];
    char path[1024];
    sprintf(link, "/proc/%d/exe",getpid());
    readlink(link, path, sizeof(path));

    m_exe = path;
    auto pos = m_exe.find_last_of("/");
    m_cwd = m_exe.substr(0, pos) + "/";

    m_program = argv[0];

    const char* now_key = nullptr;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (strlen(argv[i]) > 1) {
                if (now_key) {
                    add(now_key, "");
                }
                now_key = argv[i] + 1;
            } else {
                FANG_LOG_ERROR(g_logger) << "invalid arg idx=" << i
                    << " val=" << argv[i];
                return false;
            }
        } else {
            if (now_key) {
                add(now_key, argv[i]);
                now_key = nullptr;
            } else {
                FANG_LOG_ERROR(g_logger) << "invalid arg idx=" << i
                    << " val=" << argv[i];
                return false;
            }
        }
    }
    if (now_key) {
        add(now_key, "");
    }
    return true;
}

void Env::add(const std::string& key, const std::string& val) {
    RWMutexType::WrLock lock(m_mutex);
    m_args[key] = val;
}
bool Env::has(const std::string& key) {
    RWMutexType::RdLock lock(m_mutex);
    auto it = m_args.find(key);
    return it != m_args.end();
}
void Env::del(const std::string& key) {
    RWMutexType::WrLock lock(m_mutex);
    m_args.erase(key);
}

std::string Env::get(const std::string& key, const std::string& default_value) {
    RWMutexType::RdLock lock(m_mutex);
    auto it = m_args.find(key);
    return it != m_args.end() ? it->second : default_value;
}

void Env::addHelp(const std::string& key, const std::string& desc) {
    remoteHelp(key);
    RWMutexType::WrLock lock(m_mutex);
    m_helps.push_back(std::make_pair(key, desc));
}
void Env::remoteHelp(const std::string& key) {
    RWMutexType::WrLock lock(m_mutex);
    for (auto it = m_helps.begin();
            it != m_helps.end(); ++it) {
        if (it->first == key) {
            it = m_helps.erase(it);
        }
    }
}

void Env::printHelp() {
    RWMutexType::RdLock lock(m_mutex);
    std::cout << "Usage: " << m_program << " [options]" << std::endl;
    for (auto& i : m_helps) {
        std::cout << std::setw(5) << "-" << i.first << " : " << i.second << std::endl;
    }
}

bool Env::setEnv(const std::string& key, const std::string& val) {
    return !setenv(key.c_str(), val.c_str(), 1);
}
std::string Env::getEnv(const std::string& key, const std::string& default_value) {
    const char* v = getenv(key.c_str());
    return v != nullptr ? v : default_value;
}

std::string Env::getAbsolutePath(const std::string& path) const {
    if (path.empty()) {
        return "/";
    }
    if (path[0] == '/') {
        return path;
    }
    return m_cwd + path;
}

std::string Env::getAbsoluteWorkPath(const std::string& path) const {
    if (path.empty()) {
        return "/";
    }
    if (path[0] == '/') {
        return path;
    }
    static fang::ConfigVar<std::string>::ptr g_server_work_path = 
        fang::Config::Lookup<std::string>("server.worker_path");
    return g_server_work_path->getValue() + "/" + path;
}

std::string Env::getConfigPath() {
    return getAbsolutePath(get("c", "conf"));

}

}
