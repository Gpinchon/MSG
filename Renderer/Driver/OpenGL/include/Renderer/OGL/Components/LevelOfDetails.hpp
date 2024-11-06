#pragma once

#include <Renderer/OGL/Components/MeshData.hpp>

#include <vector>

namespace TabGraph::Renderer::Component {
class LevelOfDetails {
public:
    std::vector<PrimitiveList> levels;
    std::vector<float> screenCoverage;
};
}
