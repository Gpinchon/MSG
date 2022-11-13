#pragma once

#include "GameEntity.hpp"

#include <Events/Signal.hpp>

#include <chrono>

class Flame : public GameEntity {
public:
    Flame(Level&);
    ~Flame();
    static std::shared_ptr<Flame> Create(Level& level, const glm::ivec2& position);
    std::chrono::time_point<std::chrono::high_resolution_clock> SpawnTime() const;
    std::chrono::duration<double> Timer() const;
    void SetTimer(std::chrono::duration<double> timer);
    virtual void Update(float delta) override;

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return std::make_shared<Flame>(*this);
    }
    std::chrono::duration<double> _timer { 0.5 }; //defaults to seconds
    const std::chrono::time_point<std::chrono::high_resolution_clock> _spawnTime;
    Signal<float>::ScoppedSlot _updateSlot;
};