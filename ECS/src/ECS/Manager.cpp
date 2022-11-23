/*
* @Author: gpinchon
* @Date:   2021-07-27 20:54:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-27 20:55:21
*/
#include <ECS/Manager.hpp>

namespace TabGraph::ECS {
Manager& Manager::_Get()
{
    static Manager manager;
    return manager;
};
}
