#pragma once

#include "GameEntity.hpp"

#include <Events/Event.hpp>

class Player : public GameEntity {
public:
    Player(Level& level, const std::string& name, const glm::vec3& color);
    static std::shared_ptr<Player> Create(Level& level, const glm::vec3& color);
    void Move(const glm::vec2& direction, float delta);
    float Speed() const;
    void SetSpeed(float speed);
    void DropBomb(const Event::Keyboard& event) const;
    virtual void Update(float delta) override;
    virtual void Die() override;

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return std::shared_ptr<Player>(new Player(*this));
    }
    float _speed { 4.f };
    uint8_t _lives { 3 };
};