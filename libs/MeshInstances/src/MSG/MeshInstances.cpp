#include <MSG/MeshInstances.hpp>

#include <MSG/Mesh.hpp>
#include <MSG/Mesh/Skin.hpp>

void Msg::MeshInstances::ComputeBoundingVolume(const Mesh* a_Mesh, const MeshSkin* a_MeshSkin)
{
    boundingVolume = { };
    for (uint32_t i = 0; i < instances; i++) {
        if (a_Mesh != nullptr)
            boundingVolume += transforms[i] * a_Mesh->boundingVolume;
        if (a_MeshSkin != nullptr)
            boundingVolume += transforms[i] * a_MeshSkin->boundingVolume;
    }
}