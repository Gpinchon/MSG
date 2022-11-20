#include <SG/Visitor/SerializeVisitor.hpp>

#include <SG/Node/Node.hpp>

namespace TabGraph::SG {
void SerializeVisitor::_Visit(Object& a_Object) {
    _ostream << "{\n";
    _ostream << a_Object;
    _ostream << "}\n";
}
}