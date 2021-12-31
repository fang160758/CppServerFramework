#include "../inc/module.h"
#include "../inc/env.h"
#include "../inc/log.h"
#include "../inc/config.h"
#include <algorithm>
#include <string>
#include <vector>

namespace fang {

static fang::Logger::ptr g_logger = FANG_LOG_NAME("system");
static fang::ConfigVar<std::string>::ptr g_module_path
    = fang::Config::Lookup("module.path", std::string("module"), "module path");

Module(const std::string& name
     , const std::string& version
     , const std::string& filename
     , uint32_t type = MODULE)
    : m_name(name)
    , m_version(version)
    , m_filename(filename)
    , m_id(name + "/" + version)
    , m_type(type) {}


void Module::onBeforeArgsParse(int argc, char** argv) {}

void Module::onAfterArgsParse (int argc, char** argv) {}

bool Module::onLoad() { return true; }

bool Module::onUnload() { return true; }

bool Module::onConnect   (fang::Stream::ptr stream) { return true; }

bool Module::onDisconnect(fang::Stream::ptr stream) { return true; }

bool Module::onServerReady() { return true; }

bool Module::onServerUp() { return true; }

bool Module::handleRequest(fang::Message::ptr req
                         , fang::Message::ptr rsp
                         , fang::Stream::ptr stream) { return true; }

bool Module::handleNotify(fang::Message::ptr notify
                        , fang::Stream::ptr stream) { return true; }

std::string Module::statusString() {
    std::stringstream ss;
    ss << "Module name=" << getName()
       << " version=" << getVersion()
       << " filename=" << getFilename()
       << std::endl;
    return ss.str();
}

void Module::registerService(const std::string& server_type
                           , const std::string& domain
                           , const std::string& service) {
}


ModuleManager::ModuleManager() {}

void ModuleManager::add(Module::ptr m) {
    del(m->getId());
    RWMutexType::WrLock lock(m_mutex);
    m_modules[m->getId()] = m;
    m_type2Modules[m->getType()][m->getId()] = m;
}

void ModuleManager::del(const std::string& name) {
    Module::ptr module;
    RWMutexType::WrLock lock(m_mutex);
    auto it = m_modules.find(name);
    if (it == m_modules.end()) {
        return;
    }
    module = it->second;
    m_modules.erase(it);
    m_type2Modules[module->getType()].erase(module->getId());
    if (m_type2Modules[module->getType()].empty()) {
        m_type2Modules.erase(module->getType());
    }
    lock.unlock();
    module->onUnload();
}

void ModuleManager::delAll() {
    RWMutexType::RdLock lock(m_mutex);
    auto tmp = m_modules;
    lock.unlock();

    for (auto& i : tmp) {
        del(i.first);
    }
}

void ModuleManager::init() {
    auto path = fang::EnvMgr::GetInstance()->getAbsolutePath(g_module_path->getValue());

    std::vector<std::string> files;
    fang::Helpc::ListAllFile(files, path, ".so");

    std::sort(files.begin(), files.end());
    for(auto& i : files) {
        initModule(i);
    }
}

Module::ptr ModuleManager::get(const std::string& name) {
    RWMutexType::RdLock lock(m_mutex);
    auto it = m_modules[name];
    return it == m_modules.end() ? nullptr : it->second;
}

void ModuleManager::onConnect(Stream::ptr stream) {
    std::vector<Module::ptr> ms;
    listAll(ms);
    for (auto& m : ms) {
        m->onConnect(stream);
    }
}
void ModuleManager::onDisconnect(Stream::ptr stream) {
    std::vector<Module::ptr> ms;
    listAll(ms);
    for (auto& m : ms) {
        m->onDisconnect(stream);
    }
}

void ModuleManager::listAll(std::vector<Module::ptr>& ms) {
    RWMutexType::RdLock lock(m_mutex);
    for(auto& i : m_modules) {
        ms.push_back(i.second);
    }
}

void ModuleManager::listByType(uint32_t type, std::vector<Module::ptr>& ms) {
    RWMutexType::RdLock lock(m_mutex);
    auto it = m_type2Modules.find(type);
    if (it == m_type2Modules.end()) {
        return nullptr;
    }
    for (auto& i : it->second) {
        ms.push_back(i.second);
    }
}

void ModuleManager::foreach(uint32_t type, std::function<void(Module::ptr)> cb) {
    std::vector<Module::ptr> ms;
    listByType(type, ms);
    for(auto& i : ms) {
        cb(i);
    }
}

void ModuleManager::initModule(const std::string& path) {
    Module::ptr m = Library::GetModule(path);
    if (m) {
        add(m);
    }
}

}
