#pragma once

#include "GameEntity.hpp"

#include <chrono>

class Material;

class Bomb : public GameEntity {
public:
    Bomb(Level &);
    Bomb(const Bomb&);
    static std::shared_ptr<Bomb> Create(Level& level, const glm::ivec2& position);
    virtual void Die() override;
    void ResetTimer();
    std::chrono::time_point<std::chrono::high_resolution_clock> SpawnTime() const;
    std::chrono::duration<double> Timer() const;
    void SetTimer(std::chrono::duration<double> timer);
    int Range() const;
    void SetRange(int range);
    ~Bomb() { std::cout << "BOOM !" << std::endl; };
    virtual void Update(float delta) override;

private:
    void _SpawnFlames(const glm::ivec2& position, const glm::ivec2& direction, int range);
    int _range { 5 };
    std::chrono::duration<double> _timer { 3 };
    std::chrono::time_point<std::chrono::high_resolution_clock> _spawnTime;
    std::shared_ptr<Material> _bombMaterial;
};