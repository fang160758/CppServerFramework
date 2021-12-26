#include "../inc/config.h"
#include "../inc/env.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <sys/stat.h>


namespace fang {
static fang::Logger::ptr g_logger = FANG_LOG_NAME("system");

ConfigVarBase::ptr Config::LookupBase(const std::string& name) {
    RWMutexType::RdLock lock(GetMutex());
    auto it = GetDatas().find(name);
    return it == GetDatas().end() ? nullptr : it->second;
}

void Config::Visit(std::function<void(ConfigVarBase::ptr)> cb) {
    RWMutexType::RdLock lock(GetMutex());
    ConfigVarMap& m = GetDatas();
    for (auto it = m.begin();
            it != m.end(); ++it) {
        cb(it->second);
    }
}

static void ListAllMember(const std::string& prefix
        , const YAML::Node& node
        , std::list<std::pair<std::string, const YAML::Node> >& output) {
    if (prefix.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678")
            != std::string::npos) {
        FANG_LOG_ERROR(g_logger) << "Config invalid name: " << prefix
            << " : " << node;
        return;
    }
    output.push_back(std::make_pair(prefix, node));
    if (node.IsMap()) {
        for (auto it = node.begin();
                it != node.end(); ++it) {
            ListAllMember(prefix.empty() ? it->first.Scalar()
                    : prefix + "." + it->first.Scalar(), it->second, output);
        }
    }
}


void Config::LoadFromYaml(const YAML::Node& root) {
    std::list<std::pair<std::string, const YAML::Node> > all_nodes;
    ListAllMember("", root, all_nodes);
    
    for (auto& i : all_nodes) {
        std::string key = i.first;
        if (key.empty()) {
            continue;
        }

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = LookupBase(key);
        
        if (var) {
            if (i.second.IsScalar()) {
                var->fromString(i.second.Scalar());
            } else {
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}

static std::map<std::string, uint64_t> s_file2modifytime;
static fang::Mutex s_mutex;

void Config::LoadFromConfDir(const std::string& path, bool force) {
    std::string absoulte_path = EnvMgr::GetInstance()->getAbsolutePath(path);
    std::vector<std::string> files;
    Helpc::ListAllFile(files, absoulte_path, ".yml");

    for (auto& i : files) {
        {
            struct stat st;
            lstat(i.c_str(), &st);
            fang::Mutex::Lock lock(s_mutex);
            if (!force && s_file2modifytime[i] == (uint64_t)st.st_mtime) {
                continue;
            }
            s_file2modifytime[i] = st.st_mtime;
        }
        try {
            YAML::Node root = YAML::LoadFile(i);
            LoadFromYaml(root);
            FANG_LOG_INFO(g_logger) << "LoadConfigFile file=" 
                << i << " ok";
        } catch (...) {
            FANG_LOG_ERROR(g_logger) << "LoadConfigFile  file="
                << i << " failed";
        }
    }
}


}
