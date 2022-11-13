/*
* @Author: gpinchon
* @Date:   2020-08-08 19:59:48
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 22:00:47
*/
#pragma once

#include "Scene/Scene.hpp"

#include <filesystem>

class GameEntity;
class Player;

class Level : public Scene {
public:
    Level(const std::string& name, const glm::ivec2& size);
    ~Level();
    static std::shared_ptr<Level> Create(const std::filesystem::path& path);
    glm::ivec2 Size() const;
    void SetGameEntity(glm::ivec2 position, std::shared_ptr<GameEntity> entity);
    void SetGameEntityPosition(glm::ivec2 position, std::shared_ptr<GameEntity> entity);
    std::shared_ptr<GameEntity> GetGameEntity(glm::ivec2 position) const;
    void SetSpawnPoint(glm::ivec2);
    glm::ivec2 SpawnPoint() const;
    void Update(float delta);

private:    
    const glm::ivec2 _size;
    glm::ivec2 _spawnPoint;
    std::vector<std::weak_ptr<GameEntity>> _entities;
    std::vector<std::weak_ptr<Player>> _players;
};