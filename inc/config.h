#pragma once
#include "log.h"
#include "mutex.h"
#include "helpc.h"
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <list>
#include <memory>
#include <pthread.h>
#include <stdexcept>
#include <string>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/node/type.h>
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <functional>

namespace fang {

class ConfigVarBase {
    
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;

    ConfigVarBase(const std::string& name, const std::string& description = "")
        : m_name(name)
        , m_description(description) {
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
        }

    virtual ~ConfigVarBase() {}

    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }

    virtual std::string toString() = 0;

    virtual bool fromString(const std::string& val) = 0;

    virtual std::string getTypeName() const = 0;

protected:
    std::string m_name;
    std::string m_description;
};

template<class F, class T>
class LexicalCast {
public:
    
    T operator()(const F& v) {
        return boost::lexical_cast<T>(v);
    }
};

template<class T>
class LexicalCast<std::string, std::vector<T> > {
public:
    std::vector<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::vector<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator()(const std::vector<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::list<T> > {
public:
    std::list<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::list<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::list<T>, std::string> {
public:
    std::string operator()(const std::list<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::set<T> > {
public:
    std::set<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::set<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator()(const std::set<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};


template<class T>
class LexicalCast<std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::unordered_set<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};


template<class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator()(const std::unordered_set<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::map<std::string, T> vec;
        std::stringstream ss;
        for (auto it = node.begin();
                it != node.end(); ++ it) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::map<std::string, T> , std::string> {
public:
    std::string operator()(const std::map<std::string, T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::unordered_map<std::string, T> vec;
        std::stringstream ss;
        for (auto it = node.begin();
                it != node.end(); ++ it) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::unordered_map<std::string, T> , std::string> {
public:
    std::string operator()(const std::unordered_map<std::string, T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};


template<class T, class FromStr = LexicalCast<std::string, T>
                , class ToStr = LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarBase {
public:
    typedef RWMutex RWMutexType;
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb;

    ConfigVar(const std::string& name
            , const T& default_value
            , const std::string& description = "")
        : ConfigVarBase(name, description)
        , m_value(default_value) {}

    std::string toString() override {
        try {
            RWMutexType::RdLock lock(m_mutex);
            return ToStr()(m_value);
        } catch (std::exception& e) {
            FANG_LOG_ERROR(FANG_LOG_ROOT()) << "ConfigVar::toString exception "
                << e.what() << " convert: " << TypeToName<T>() << " to string"
                << " name=" << m_name;            
        }
        return "";
    }

    bool fromString(const std::string& val) override {
        try {
            setValue(FromStr(val));
        } catch (std::exception& e) {
            FANG_LOG_ERROR(FANG_LOG_ROOT()) << "ConfigVar::fromString exception "
                << e.what() << " convert: string to" << TypeToName<T>()
                << " name=" << m_name
                << " - " << val;
        }
        return false;
    }

    const T getValue() { 
        RWMutexType::RdLock lock(m_mutex);
        return m_value;
    }

    void setValue(const T& v) {
        {
            RWMutexType::RdLock lock(m_mutex);
            if (v == m_value) {
                return;
            }
            for (auto& i : m_cbs) {
                i.second(m_value, v);
            }
        }
        RWMutexType::WrLock lock(m_mutex);
        m_value = v;
    }

    std::string getTypeName() const override { return TypeToName<T>(); }


    uint64_t addListener(on_change_cb cb) {
        static uint64_t s_fun_id = 0;
        RWMutexType::WrLock lock(m_mutex);
        ++s_fun_id;
        m_cbs[s_fun_id] = cb;
        return s_fun_id;
    }

    void delListener(uint64_t key) {
        RWMutexType::WrLock lock(m_mutex);
        m_cbs.erase(key);
    }

    on_change_cb getListener(uint64_t key) {
        RWMutexType::RdLock lock(m_mutex);
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }

    void clearListener() {
        RWMutexType::WrLock lock(m_mutex);
        m_cbs.clear();
    }


private:
    RWMutexType m_mutex;
    T m_value;
    std::map<uint64_t, on_change_cb> m_cbs;
};


class Config {
public:
    typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    typedef RWMutex RWMutexType;


    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name
            , const T& default_value, const std::string& description = "") {
        RWMutexType::WrLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if (it != GetDatas().end()) {
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
            if (tmp) {
                FANG_LOG_INFO(FANG_LOG_ROOT()) << "Lookup name=" << name << " exists";
                return tmp;
            } else {
                FANG_LOG_ERROR(FANG_LOG_ROOT()) << "Lookup name=" << name << " exists but type not "
                    << TypeToName<T>() << " real_type=" << it->second->getTypeName()
                    << " " << it->second->toString();
                return nullptr;
            }
        }

        if (name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678") 
                != std::string::npos) {
            FANG_LOG_ERROR(FANG_LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        GetDatas()[name] = v;
        return v;
    }
    
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        RWMutexType::RdLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if (it == GetDatas().end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    } 


    static void LoadFromYaml(const YAML::Node& root);
    static void LoadFromConfDir(const std::string& path, bool force = false);
    static ConfigVarBase::ptr LookupBase(const std::string& name);
    static void Visit(std::function<void(ConfigVarBase::ptr)> cb);
    
private:
    static ConfigVarMap& GetDatas() {
        static ConfigVarMap s_datas;
        return s_datas;
    }

    static RWMutexType& GetMutex() {
        static RWMutexType s_mutex;
        return s_mutex;
    }
};


} 