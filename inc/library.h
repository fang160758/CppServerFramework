#pragma once
#include "module.h"
#include <string>
namespace fang {
class Library {
public:
    static Module::ptr GetModule(const std::string& path);
};

}
