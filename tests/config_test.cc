#include "../inc/config.h"
#include "../inc/env.h"
#include "../inc/log.h"
#include <sstream>
#include <string>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
static fang::Logger::ptr g_logger = FANG_LOG_NAME("test");

class Person {
public:
    Person() {};
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;

    std::string toString() const {
        std::stringstream ss;
        ss << "[Person name=" << m_name
           << " age=" << m_age
           << " sex=" << (m_sex == 0 ? "男" : "女")
           << "]";
        return ss.str();
    }

    bool operator==(const Person& oth) const {
        return m_name == oth.m_name
            && m_age == oth.m_age
            && m_sex == oth.m_sex;
    }
};

namespace fang {
template<>
class LexicalCast<std::string, Person> {
public:
    Person operator()(const std::string& v) {
        YAML::Node node= YAML::Load(v);
        Person  p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

template<>
class LexicalCast<Person, std::string> {
public:
    std::string operator()(const Person& p) {
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};
}


fang::ConfigVar<Person>::ptr g_person
    = fang::Config::Lookup("class.person", Person(), "system person");

fang::ConfigVar<std::map<std::string, Person> >::ptr g_person_map
    = fang::Config::Lookup("class.map", std::map<std::string, Person>(), "system person");

fang::ConfigVar<std::map<std::string, std::vector<Person> > >::ptr g_person_vec_map 
    = fang::Config::Lookup("class.vec_map", std::map<std::string, std::vector<Person> >(), "system Person");

void test_class() {
    //FANG_LOG_INFO(g_logger) << "before: " << g_person->getValue().toString() << " - " << g_person->toString();
    //FANG_LOG_INFO(g_logger) << "before: " << g_person_map->toString();
    //FANG_LOG_INFO(g_logger) << "before: " << g_person_vec_map->toString();

#define XX_M(g_var, prefix){\
        auto& v = g_var->getValue();\
        for (auto& i : v) {\
            FANG_LOG_INFO(g_logger) << prefix << ": " << i.first << " - " << i.second.toString();\
        }\
        FANG_LOG_INFO(g_logger) << prefix << ": size=" << v.size();\
    }

g_person->addListener([](const Person& old_value, const Person& new_value){
        FANG_LOG_INFO(g_logger) << "old_value=" << old_value.toString()
                << " new_value=" << new_value.toString();
        });
    XX_M(g_person_map, "class.map before");
    FANG_LOG_INFO(g_logger) << g_person_vec_map->toString();
    fang::Config::LoadFromConfDir("conf");

    XX_M(g_person_map, "class.map after");
    FANG_LOG_INFO(g_logger) << g_person_vec_map->toString();
}

void test_loadconf() {
    fang::Config::LoadFromConfDir("conf");
}

int main(int argc, char** argv) {
//    fang::EnvMgr::GetInstance()->init(argc, argv);
//    test_loadconf();
test_class();
}
