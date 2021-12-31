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
#include <stdexcept>
#include <string.h>
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
    
    /**
     * @Synopsis  构造函数
     *
     * @Param[in] name 配置参数的名称[0-9a-z_.]
     * @Param[in] description 配置参数的描述
     */
    ConfigVarBase(const std::string& name, const std::string& description = "")
        : m_name(name)
        , m_description(description) {
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
        }
    
    /**
     * @Synopsis  虚析构函数
     */
    virtual ~ConfigVarBase() {}

    /**
     * @Synopsis  返回配置参数的名字
     *
     * @Returns   配置参数的名称
     */
    const std::string& getName() const { return m_name; }

    /**
     * @Synopsis  返回配置参数的描述
     *
     * @Returns   配置参数的描述
     */
    const std::string& getDescription() const { return m_description; }

    /**
     * @Synopsis  转成字符串
     */
    virtual std::string toString() = 0;

    /**
     * @Synopsis  从字符串初始化配置参数
     */
    virtual bool fromString(const std::string& val) = 0;

    /**
     * @Synopsis  返回配置参数的类型名称
     */
    virtual std::string getTypeName() const = 0;

protected:
    // 配置参数的名称
    std::string m_name;

    // 配置参数的描述
    std::string m_description;
};


/**
 * @Synopsis 类型转换模版类
 *
 * @tparam F 源类型
 * @tparam T 目标类型
 */
template<class F, class T>
class LexicalCast {
public:
    T operator()(const F& v) {
        return boost::lexical_cast<T>(v);
    }
};

/**
 * @Synopsis  模版片特化(std::string ---> std::vector<T>)
 */
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

/**
 * @Synopsis  模版片特化(std::vector<T> ----> std::string)
 */
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

/**
 * @Synopsis  模版片特化(std::string ----> std::list<T>)
 */
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

/**
 * @Synopsis  模版片特化(std::list<T> ----> std::string)
 */
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

/**
 * @Synopsis  模版片特化(std::string ----> std::set<T>)
 */
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

/**
 * @Synopsis  模版片特化(std::set<T> ----> std::string)
 */
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

/**
 * @Synopsis  模版片特化(std::string ----> std::unordered_set<T>)
 */
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

/**
 * @Synopsis  模版片特化(std::unordered_set<T> ----> std::string)
 */
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

/**
 * @Synopsis  模版片特化(std::string ----> std::map<std::string, T>)
 */
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

/**
 * @Synopsis  模版类片特化(std::map<std::string, T> ----> std::string)
 */
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

/**
 * @Synopsis  模版片特化(std::string ----> std::unordered_map<std::string, T>)
 */
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

/**
 * @Synopsis  模版类片特化(std::unordered_map<std::string, T> ----> std::string)
 */
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

/**
 * @Synopsis  配置参数模版子类
 *
 * @tparam T  模版类型
 * @tparam FromStr 转换模版类(std::string ----> T)
 * @tparam ToStr 转换模版类(T ----> std::string)
 */
template<class T, class FromStr = LexicalCast<std::string, T>
                , class ToStr = LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarBase {
public:
    typedef RWMutex RWMutexType;
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb;


    /**
     * @Synopsis  构造函数
     *
     * @Param[in] name 参数的名字
     * @Param[in] default_value 参数的默认值
     * @Param[in] description 参数的描述
     */
    ConfigVar(const std::string& name
            , const T& default_value
            , const std::string& description = "")
        : ConfigVarBase(name, description)
        , m_value(default_value) {}

    /**
     * @Synopsis  将参数转换为YAML string
     *
     * @Exception 当转换失败会抛出异常
     */
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

    /**
     * @Synopsis  从YAML string 转换为 参数配置类
     *
     * @Param[in] val YAML string
     *
     * @Exception 抛出异常
     */
    bool fromString(const std::string& val) override {
        try {
            setValue(FromStr()(val));
        } catch (std::exception& e) {
            FANG_LOG_ERROR(FANG_LOG_ROOT()) << "ConfigVar::fromString exception "
                << e.what() << " convert: string to" << TypeToName<T>()
                << " name=" << m_name
                << " - " << val;
        }
        return false;
    }

    /**
     * @Synopsis  获取当前参数的值
     */
    const T getValue() { 
        RWMutexType::RdLock lock(m_mutex);
        return m_value;
    }

    /**
     * @Synopsis  设置当前参数的值，如果参数的值发生变化就通知对应的回调函数
     *
     * @Param[in] v T类型的值
     */
    void setValue(const T& v) {
        {
            RWMutexType::RdLock lock(m_mutex);
            if (v == m_value) {
                return;
            }
            for (auto& i : m_cbs) {
                i.second(m_value, v); // 执行设置好的回调函数
            }
        }
        RWMutexType::WrLock lock(m_mutex);
        m_value = v;
    }
    
    /**
     * @Synopsis  获取参数的类型名称
     */
    std::string getTypeName() const override { return TypeToName<T>(); }

    /**
     * @Synopsis  添加变化的回调函数
     *
     * @Param[in] cb 回调函数指针
     *
     * @Returns   返回该回调函数的唯一索引值，方便删除该回调函数
     */
    uint64_t addListener(on_change_cb cb) {
        static uint64_t s_fun_id = 0;
        RWMutexType::WrLock lock(m_mutex);
        ++s_fun_id;
        m_cbs[s_fun_id] = cb;
        return s_fun_id;
    }

    /**
     * @Synopsis  删除key对应的回调函数
     *
     * @Param[in] key 回调函数的唯一索引
     */
    void delListener(uint64_t key) {
        RWMutexType::WrLock lock(m_mutex);
        m_cbs.erase(key);
    }

    /**
     * @Synopsis  获取key对应的回调函数
     *
     * @Param[in] key 回调函数的唯一索引
     */
    on_change_cb getListener(uint64_t key) {
        RWMutexType::RdLock lock(m_mutex);
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }

    /**
     * @Synopsis  清除所有的回调函数
     */
    void clearListener() {
        RWMutexType::WrLock lock(m_mutex);
        m_cbs.clear();
    }


private:
    // 对写锁
    RWMutexType m_mutex;
    // 配置参数 T类型的值
    T m_value;
    // 回调函数哈希表
    std::map<uint64_t, on_change_cb> m_cbs;
};


/**
 * @Synopsis  配置参数的管理类，提供便捷的方法创建/管理ConfigVar
 */
class Config {
public:
    typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    typedef RWMutex RWMutexType;


    /**
     * @Synopsis  外放接口，获取指定名称的配置参数类，如果配置参数容器内没有该参数且参数名称合法就创建一个ConfigVar
     *
     * @Param[in] name 配置参数名称
     * @Param[in] default_value 参数默认值
     * @Param[in] description 参数描述
     *
     * @Returns   返回ConfigVar类
     */
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name
            , const T& default_value, const std::string& description = "") {
        RWMutexType::WrLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if (it != GetDatas().end()) {
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second); // 基类动态转化为派生类
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
    
    /**
     * @Synopsis  查找配置参数
     *
     * @Param[in] name 参数名称
     *
     * @Returns   配置参数类
     */
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        RWMutexType::RdLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if (it == GetDatas().end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    } 

    /**
     * @Synopsis  使用YAML::Node 初始化配置模块
     */
    static void LoadFromYaml(const YAML::Node& root);
    
    /**
     * @Synopsis  加载path文件夹里面的配置文件
     */
    static void LoadFromConfDir(const std::string& path, bool force = false);
    
    /**
     * @Synopsis  查找配置参数，返回基类
     *
     * @Param[in] name 配置参数名称
     */
    static ConfigVarBase::ptr LookupBase(const std::string& name);
    
    /**
     * @Synopsis  遍历配置模块里面所有配置项
     *
     * @Param[in] cb 配置项回调函数
     */
    static void Visit(std::function<void(ConfigVarBase::ptr)> cb);
    
private:

    /**
     * @Synopsis  返回配置参数的容器
     */
    static ConfigVarMap& GetDatas() {
        static ConfigVarMap s_datas;
        return s_datas;
    }

    /**
     * @Synopsis  返回全局的读写锁
     */
    static RWMutexType& GetMutex() {
        static RWMutexType s_mutex;
        return s_mutex;
    }
};


} 
