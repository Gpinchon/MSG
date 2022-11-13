/*
* @Author: gpinchon
* @Date:   2020-08-08 20:02:06
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-16 18:17:12
*/

#include "GameEntity.hpp"
#include "Level.hpp"

#include <Animation/Animation.hpp>

GameEntity::GameEntity(const GameEntity& entity) : Node(entity)
    , _size(entity._size)
    , _height(entity._height)
    , _type(entity._type)
    , _animations(entity._animations)
    , _currentAnimation(entity._currentAnimation)
    , _level(entity._level)
{
}

GameEntity::GameEntity(Level& level, const std::string& name, const std::string& entityType)
    : Node(name)
    , _type(entityType)
    , _level(level)
{
}

GameEntity::GameEntity(Level& level, const std::string& entityType)
    : Node("")
    , _type(entityType)
    , _level(level)
{
}

GameEntity::~GameEntity()
{
    for (auto& anim: _animations) {
        auto animation{ anim.second.lock() };
        if (animation == nullptr)
            continue;
        _level.RemoveComponent(animation);
    }
}

std::string GameEntity::Type() const
{
    return _type;
}

glm::vec2 GameEntity::Size() const
{
    return _size;
}

float GameEntity::Height() const
{
    return _height;
}

void GameEntity::SetPosition(const glm::vec2& position)
{
    auto pos3D = glm::vec3(position.x, Height() / 2.f, position.y);
    Node::SetPosition(pos3D);
    //GetComponent<RigidBody>()->CurrentTransform().SetPosition(pos3D);
    //GetComponent<RigidBody>()->NextTransform().SetPosition(pos3D);
}

void GameEntity::LookAt(const glm::vec2& position)
{
    auto pos3D = glm::vec3(position.x, Height() / 2.f, position.y);
    Node::LookAt(pos3D);
}

glm::vec2 GameEntity::Position() const
{
    auto position = WorldPosition();
    return glm::vec2(position.x, position.z);
}

void GameEntity::Die()
{
}

void GameEntity::AddAnimation(const std::shared_ptr<Animation> &animation)
{
    //AddComponent(animation);
    _animations[animation->GetName()] = animation;
    _level.Add(animation);
}

void GameEntity::PlayAnimation(const std::string& name, bool repeat)
{
    auto animation = _animations.at(name).lock();
    if (animation != nullptr && !animation->Playing()) {
        animation->SetRepeat(repeat);
        animation->Play();
        auto currentAnimation = _currentAnimation.lock();
        if (currentAnimation != nullptr) {
            currentAnimation->Stop();
        }
        _currentAnimation = animation;
    }
}
