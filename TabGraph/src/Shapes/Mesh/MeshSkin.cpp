#include <Shapes/Mesh/MeshSkin.hpp>

#include <algorithm>

namespace TabGraph::Shapes {
Mesh::Skin::Skin()
    : Inherit()
{
    static size_t meshSkinCount(0);
    SetName(GetClassName() + std::to_string(++meshSkinCount));
}

}
