#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Animation/Channel.hpp>
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Core/Property.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Animation : public Core::Inherit<Core::Object, Animation> {
public:
    enum class LoopMode {
        Swing,
        Repeat
    };
    READONLYPROPERTY(bool, Playing, false);
    PROPERTY(bool, Loop, false);
    PROPERTY(LoopMode, LoopMode, LoopMode::Swing);
    PROPERTY(float, Speed, 1);

public:
    Animation();
    void AddChannelPosition(const AnimationChannel<glm::vec3>& a_AnimationChannel)
    {
        _positions.push_back(a_AnimationChannel);
    }
    void AddChannelScale(const AnimationChannel<glm::vec3>& a_AnimationChannel)
    {
        _scales.push_back(a_AnimationChannel);
    }
    void AddChannelRotation(const AnimationChannel<glm::quat>& a_AnimationChannel)
    {
        _rotations.push_back(a_AnimationChannel);
    }

    auto& GetChannelPosition(size_t index)
    {
        return _positions.at(index);
    }
    auto& GetChannelScale(size_t index)
    {
        return _scales.at(index);
    }
    auto& GetChannelRotation(size_t index)
    {
        return _rotations.at(index);
    }
    /** @brief Start playing the animation */
    void Play();
    /** @brief Advance the animation */
    void Advance(float delta);
    /** @brief Stop the animation */
    void Stop();
    /** @brief Resets the animation to the beginning */
    void Reset();

private:
    std::vector<AnimationChannel<glm::vec3>> _positions;
    std::vector<AnimationChannel<glm::vec3>> _scales;
    std::vector<AnimationChannel<glm::quat>> _rotations;
    float _currentTime { 0 };
};
}
