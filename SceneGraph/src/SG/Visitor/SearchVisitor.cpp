/*
* @Author: gpinchon
* @Date:   2021-06-20 13:29:09
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-20 13:29:56
*/
#include <SG/Node/Node.hpp>
#include <SG/Visitor/SearchVisitor.hpp>

namespace TabGraph::SG {
SearchVisitor::SearchVisitor(const std::string& name, const NodeVisitor::Mode& mode)
    : NodeVisitor(mode)
    , by(SearchVisitor::By::Name)
    , _functor([name](SearchVisitor& search, Object& object) {
        return name == object.GetName();
    })
{
}
SearchVisitor::SearchVisitor(const uint32_t& id, const NodeVisitor::Mode& mode)
    : NodeVisitor(mode)
    , by(SearchVisitor::By::Id)
    , _functor([id](SearchVisitor& search, Object& object) {
        return id == object.GetId();
    })
{
}
SearchVisitor::SearchVisitor(const std::type_info& type, const NodeVisitor::Mode& mode)
    : NodeVisitor(mode)
    , by(SearchVisitor::By::Type)
    , _functor([typeIndex = std::type_index(type)](SearchVisitor& search, Object& object) {
        return object.IsCompatible(typeIndex);
    })
{
}
}
