#pragma once

#include "GameEntity.hpp"

class Wall : public GameEntity {
public:
    Wall(Level& level);
    static std::shared_ptr<Wall> Create(Level& level);
    virtual void Update(float) override {};

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return std::shared_ptr<Wall>(new Wall(*this));
    }
};