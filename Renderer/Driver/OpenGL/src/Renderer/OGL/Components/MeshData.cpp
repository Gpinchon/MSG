#include <Renderer/OGL/Components/MeshData.hpp>
#include <Renderer/OGL/Primitive.hpp>
#include <Renderer/OGL/Renderer.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
//#include <Renderer/OGL/GLSL/TransformUBO.hpp>

#include <SG/Component/Mesh.hpp>
#include <Tools/LazyConstructor.hpp>

namespace TabGraph::Renderer::Component {
//MeshData::MeshData(const Renderer::Handle& a_Renderer, const SG::Component::Mesh& a_Mesh)
//{
//    for (const auto& it : a_Mesh.primitives) {
//        {
//            const auto& primitive = it.first;
//            auto [it, success]    = a_Renderer->primitives.try_emplace(primitive.get(),
//                   Tools::LazyConstructor(
//                    [&a_Renderer, &primitive]() {
//                        return std::make_shared<Primitive>(a_Renderer->context, *primitive);
//                    }));
//            auto& newPrimitive    = it->second;
//            primitives.push_back(newPrimitive);
//        }
//    }
//}
}
