#include <SG/Component/Mesh.hpp>
#include <SG/LodsGenerator.hpp>
#include <SG/PrimitiveOptimizer.hpp>
#include <Tools/ThreadPool.hpp>

#include <vector>

namespace TabGraph::SG {
Component::LevelOfDetails GenerateLods(
    const Component::Mesh& a_Mesh,
    const LodsGeneratorSettings& a_Settings)
{
    Tools::ThreadPool tp;
    std::vector<std::vector<std::shared_ptr<Primitive>>> meshLods;
    auto primitives = a_Mesh.GetPrimitives();
    meshLods.resize(primitives.size());
    for (uint64_t primitiveI = 0; primitiveI < primitives.size(); primitiveI++) {
        const auto& primitive = primitives.at(primitiveI);
        auto& primitiveLods   = meshLods.at(primitiveI);
        tp.PushCommand([currentPrimitive = primitive,
                           &lods         = primitiveLods,
                           &settings     = a_Settings]() mutable {
            SG::PrimitiveOptimizer optimizer(currentPrimitive);
            while (lods.size() < settings.lodsNbr) {
                if (optimizer.CanCompress(settings.maxCompressionError)) {
                    optimizer(settings.targetCompressionRatio, settings.maxCompressionError);
                    currentPrimitive = optimizer.result;
                }
                lods.push_back(currentPrimitive);
            }
        },
            false);
    }
    tp.Wait();
    SG::Component::LevelOfDetails levelOfDetails;
    levelOfDetails.levels.resize(a_Settings.lodsNbr);
    levelOfDetails.screenCoverage.resize(a_Settings.lodsNbr);
    // initialize lods meshes
    for (uint64_t lodI = 0; lodI < a_Settings.lodsNbr; lodI++) {
        auto& screenCoverage = levelOfDetails.screenCoverage.at(lodI);
        auto& lodMesh        = levelOfDetails.levels.at(lodI);
        screenCoverage       = 0.5f / (lodI + 1);
        lodMesh              = a_Mesh;
        lodMesh.primitives.clear();
    }
    for (uint64_t primitiveI = 0; primitiveI < meshLods.size(); primitiveI++) {
        auto& primitiveLods = meshLods.at(primitiveI);
        auto& primitive     = primitives.at(primitiveI);
        auto& material      = a_Mesh.primitives.at(primitive);
        for (uint64_t lodI = 0; lodI < primitiveLods.size(); lodI++) {
            auto& primitiveLod                                      = primitiveLods.at(lodI);
            levelOfDetails.levels.at(lodI).primitives[primitiveLod] = material;
        }
    }
    for (auto& lodMesh : levelOfDetails.levels)
        lodMesh.ComputeBoundingVolume();
    return levelOfDetails;
}
}
