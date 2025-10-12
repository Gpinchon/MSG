#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Animation/Channel.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace Msg::AnimationInterpolator {
template <typename T, typename U>
static inline T cubicSpline(T previousPoint, T prevTangent, T nextPoint, T nextTangent, U interpolationValue)
{
    auto t  = interpolationValue;
    auto t2 = t * t;
    auto t3 = t2 * t;
    return (2 * t3 - 3 * t2 + 1) * previousPoint + (t3 - 2 * t2 + t) * prevTangent + (-2 * t3 + 3 * t2) * nextPoint + (t3 - t2) * nextTangent;
}

template <typename T>
inline T Interpolate(
    const typename AnimationChannel<T>::KeyFrame& prev,
    const typename AnimationChannel<T>::KeyFrame& next,
    const AnimationInterpolation interpolation,
    const float keyDelta, const float interpolationValue)
{
    switch (interpolation) {
    case AnimationInterpolation::CubicSpline: {
        auto prevOutputTangent(prev.outputTangent);
        auto nextInputTangent(next.inputTangent);
        auto prevTangent = keyDelta * prevOutputTangent;
        auto nextTangent = keyDelta * nextInputTangent;
        return cubicSpline(prev.value, prevTangent, next.value, nextTangent, interpolationValue);
    }
    case AnimationInterpolation::Linear: {
        if constexpr (std::is_same_v<T, glm::quat>)
            return glm::slerp(prev.value, next.value, interpolationValue);
        return glm::mix(prev.value, next.value, interpolationValue);
    }
    case AnimationInterpolation::Step:
        return prev.value;
    default:
        throw std::runtime_error("Unknown animation interpolation");
    }
    return next.value;
}
}
