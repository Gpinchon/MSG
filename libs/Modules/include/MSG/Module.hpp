#pragma once

#include <typeindex>
#include <vector>

namespace MSG {
using ModuleDependencies = std::vector<std::type_index>;
class Module {
public:
    Module(const ModuleDependencies& a_Deps = {})
        : dependencies(a_Deps)
    {
    }
    const ModuleDependencies dependencies;
};
}