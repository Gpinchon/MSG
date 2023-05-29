#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <ECS/Registry.hpp>

#include <algorithm>
#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
enum class AnimationInterpolation {
    Linear,
    Step,
    CubicSpline
};
template <typename T>
struct AnimationChannel {
    struct KeyFrame {
        T inputTangent {};
        T value {};
        T outputTangent {};
        float time { 0 };
        bool operator<(const KeyFrame& other)
        {
            return time < other.time;
        }
    };
    AnimationInterpolation interpolation { AnimationInterpolation::Linear };
    size_t previousKey { 0 };
    ECS::DefaultRegistry::EntityRefType target;
    std::vector<KeyFrame> keyFrames;
    inline void InsertKeyFrame(const KeyFrame& keyFrame)
    {
        keyFrames.push_back(keyFrame);
        std::sort(keyFrames.begin(), keyFrames.end());
    }
};
}
