#include <Core/Mesh.hpp>
#include <LodsGenerator.hpp>
#include <PrimitiveOptimizer.hpp>
#include <Tools/ThreadPool.hpp>

#include <vector>

namespace MSG {
std::vector<Core::MeshLod> GenerateLods(
    const Core::Mesh& a_Mesh,
    const LodsGeneratorSettings& a_Settings)
{
    Tools::ThreadPool tp;
    auto primitives = a_Mesh.GetPrimitives(0);
    std::vector<std::future<std::vector<std::shared_ptr<Core::Primitive>>>> futures;
    futures.reserve(primitives.size());
    for (uint64_t primitiveI = 0; primitiveI < primitives.size(); primitiveI++) {
        const auto& primitive = primitives.at(primitiveI);
        futures.emplace_back(tp.Enqueue([firstPrimitive = primitive,
                                            &settings   = a_Settings]() {
            std::vector<std::shared_ptr<Core::Primitive>> lods;
            lods.reserve(settings.lodsNbr);
            std::shared_ptr<Core::Primitive> currentPrimitive = firstPrimitive;
            PrimitiveOptimizer optimizer(currentPrimitive);
            while (lods.size() < settings.lodsNbr) {
                if (optimizer.CanCompress(settings.maxCompressionError)) {
                    optimizer(settings.targetCompressionRatio, settings.maxCompressionError);
                    currentPrimitive = optimizer.result;
                }
                lods.push_back(currentPrimitive);
            }
            return lods;
        }));
    }
    std::vector<Core::MeshLod> meshLod;
    meshLod.resize(a_Settings.lodsNbr);
    for (uint64_t primitiveI = 0; primitiveI < futures.size(); primitiveI++) {
        auto primitiveLods = futures.at(primitiveI).get();
        auto& primitive    = primitives.at(primitiveI);
        auto& material     = a_Mesh.front().at(primitive);
        for (uint64_t lodI = 0; lodI < primitiveLods.size(); lodI++) {
            meshLod.at(lodI)[primitiveLods.at(lodI)] = material;
            meshLod.at(lodI).screenCoverage          = 1 - float((lodI + 1) / primitiveLods.size());
        }
    }
    return meshLod;
}
}
