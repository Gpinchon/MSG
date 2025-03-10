#include <MSG/Animation.hpp>
#include <MSG/Animation/Interpolator.hpp>
#include <MSG/Transform.hpp>

#include <glm/common.hpp>
#include <glm/gtc/quaternion.hpp>

namespace MSG {
Animation::Animation()
    : Inherit()
{
    static size_t s_animationNbr = 0;
    SetName("Animation_" + std::to_string(++s_animationNbr));
}

void Animation::Reset()
{
    _currentTime = 0;
    for (auto& channel : _scales)
        channel.previousKey = 0;
    for (auto& channel : _positions)
        channel.previousKey = 0;
    for (auto& channel : _rotations)
        channel.previousKey = 0;
}

template <typename T>
auto InterpolateChannel(AnimationChannel<T>& a_AnimationChannel, float a_CurrentTime, bool& a_AnimationPlayed)
{
    auto& minKey { a_AnimationChannel.keyFrames.front() };
    auto& maxKey { a_AnimationChannel.keyFrames.back() };
    a_CurrentTime  = std::clamp(a_CurrentTime, minKey.time, maxKey.time);
    size_t nextKey = 0;
    // TODO use range based iterations
    typename AnimationChannel<T>::KeyFrame nextKeyFrame;
    typename AnimationChannel<T>::KeyFrame prevKeyFrame;
    for (auto i = a_AnimationChannel.previousKey; i < a_AnimationChannel.keyFrames.size(); ++i) {
        auto& keyFrame(a_AnimationChannel.keyFrames.at(i));
        if (keyFrame.time > a_CurrentTime) {
            nextKey                        = std::clamp(size_t(i), size_t(0), a_AnimationChannel.keyFrames.size() - 1);
            a_AnimationChannel.previousKey = std::clamp(size_t(nextKey - 1), size_t(0), size_t(nextKey));
            nextKeyFrame                   = keyFrame;
            break;
        }
        prevKeyFrame = keyFrame;
    }
    auto keyDelta(nextKeyFrame.time - prevKeyFrame.time);
    auto interpolationValue(0.f);
    if (keyDelta != 0)
        interpolationValue = (a_CurrentTime - prevKeyFrame.time) / keyDelta;
    a_AnimationPlayed |= a_CurrentTime < maxKey.time;
    return AnimationInterpolator::Interpolate<T>(
        a_AnimationChannel.keyFrames.at(a_AnimationChannel.previousKey), a_AnimationChannel.keyFrames.at(nextKey),
        a_AnimationChannel.interpolation, keyDelta, interpolationValue);
}

void Animation::Advance(float delta)
{
    if (!GetPlaying())
        return;
    _currentTime += delta * GetSpeed();
    bool animationPlayed(false);
    for (auto& channel : _positions)
        channel.target.template GetComponent<Transform>().SetLocalPosition(InterpolateChannel(channel, _currentTime, animationPlayed));
    for (auto& channel : _scales)
        channel.target.template GetComponent<Transform>().SetLocalScale(InterpolateChannel(channel, _currentTime, animationPlayed));
    for (auto& channel : _rotations)
        channel.target.template GetComponent<Transform>().SetLocalRotation(InterpolateChannel(channel, _currentTime, animationPlayed));
    if (!animationPlayed) {
        if (GetLoop()) {
            if (GetLoopMode() == LoopMode::Repeat)
                Reset();
            else
                SetSpeed(-GetSpeed());
        } else
            Stop();
    }
}

void Animation::Play()
{
    if (!GetPlaying()) {
        _currentTime = 0;
        _SetPlaying(true);
    }
}

void Animation::Stop()
{
    Reset();
    _SetPlaying(false);
}
}
