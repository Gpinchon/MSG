/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:41
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Animation/Channel.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Animation : public Inherit<Object, Animation> {
public:
    enum class LoopMode {
        Swing, Repeat
    };
    READONLYPROPERTY(bool, Playing, false);
    PROPERTY(bool, Loop, false);
    PROPERTY(LoopMode, LoopMode, LoopMode::Swing);
    PROPERTY(float, Speed, 1);

public:
    Animation();
    void AddChannelPosition(const AnimationChannel<glm::vec3>& a_AnimationChannel) {
        _positions.push_back(a_AnimationChannel);
    }
    void AddChannelScale(const AnimationChannel<glm::vec3>& a_AnimationChannel) {
        _scales.push_back(a_AnimationChannel);
    }
    void AddChannelRotation(const AnimationChannel<glm::quat>& a_AnimationChannel) {
        _rotations.push_back(a_AnimationChannel);
    }

    auto& GetChannelPosition(size_t index) {
        return _positions.at(index);
    }
    auto& GetChannelScale(size_t index) {
        return _scales.at(index);
    }
    auto& GetChannelRotation(size_t index) {
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
