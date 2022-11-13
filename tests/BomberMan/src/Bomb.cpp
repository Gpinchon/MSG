/*
* @Author: gpinchon
* @Date:   2020-08-15 16:14:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:52:03
*/

#include <Surface/Surface.hpp>
#include <Material/Material.hpp>
#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Engine.hpp>
#include <Animation/Animation.hpp>

#include <chrono>

#include "Bomb.hpp"
#include "Flame.hpp"
#include "Game.hpp"
#include "Level.hpp"

Bomb::Bomb(Level& level)
    : GameEntity(level, "Bomb")
    , _spawnTime(std::chrono::high_resolution_clock::now())
{
    _height = 0;
}

Bomb::Bomb(const Bomb& bomb) : GameEntity(bomb)
    , _range(bomb._range)
    , _timer(bomb._timer)
    , _spawnTime(std::chrono::high_resolution_clock::now())
    , _bombMaterial(GetComponentInChildrenByName<Material>("Body"))
{
}

auto CreateBombAsset() {
    
    auto bombAsset{ std::make_shared<Assets::Asset>(Engine::GetResourcePath() / "models/bomb.gltf") };
    Assets::Parser::AddParsingTask({
        Assets::Parser::ParsingTask::Type::Sync,
        bombAsset
    });
    return bombAsset;
}

std::shared_ptr<Bomb> Bomb::Create(Level& level, const glm::ivec2& position)
{
    static auto bombAsset = CreateBombAsset();
    auto bombAssetClone{ bombAsset->GetComponent<Scene>()->Clone() };
    auto bomb = std::make_shared<Bomb>(level);
    auto bombNode = bombAssetClone->GetComponentByName<Node>("Bomb");
    bomb->AddChild(bombNode);
    for (auto& animation : bombAssetClone->GetComponents<Animation>())
        bomb->AddAnimation(animation);
    bomb->PlayAnimation("idle", true);
    bomb->_bombMaterial = bombNode->GetComponentInChildrenByName<Material>("Body");
    bomb->_spawnTime = std::chrono::high_resolution_clock::now();
    level.SetGameEntityPosition(position, bomb);
    return bomb;
}

void Bomb::_SpawnFlames(const glm::ivec2& position, const glm::ivec2& direction, int range)
{
    for (auto i = 1; i < range; ++i) {
        auto flamePosition = position + direction * i;
        if (flamePosition.x >= 0 && flamePosition.x < _level.Size().x
            && flamePosition.y >= 0 && flamePosition.y < _level.Size().y) {
            if (_level.GetGameEntity(flamePosition) == nullptr || _level.GetGameEntity(flamePosition)->Type() == "Flame")
                Flame::Create(_level, flamePosition);
            else {
                if (_level.GetGameEntity(flamePosition)->Type() == "Bomb") {
                    auto bomb = std::static_pointer_cast<Bomb>(_level.GetGameEntity(flamePosition));
                    bomb->ResetTimer();
                    bomb->SetTimer(std::chrono::duration<double>(0.01));
                    //level->GetGameEntity(flamePosition)->Die();
                } else if (_level.GetGameEntity(flamePosition)->Type() == "CrispyWall") {
                    _level.GetGameEntity(flamePosition)->Die();
                    Flame::Create(_level, flamePosition);
                }
                break;
            }
        }
    }
}

void Bomb::ResetTimer()
{
    _spawnTime = std::chrono::high_resolution_clock::now();
}

void Bomb::Die()
{
    auto position = glm::ivec2(Position());

    _SpawnFlames(position, glm::ivec2(1, 0), Range());
    _SpawnFlames(position, glm::ivec2(-1, 0), Range());
    _SpawnFlames(position, glm::ivec2(0, 1), Range());
    _SpawnFlames(position, glm::ivec2(0, -1), Range());
    Flame::Create(_level, position);
}

void Bomb::Update(float delta)
{
    auto deltaTime = std::chrono::high_resolution_clock::now() - SpawnTime();
    _bombMaterial->SetEmissive(glm::mix(glm::vec3(0), glm::vec3(0.3, 0, 0), deltaTime / Timer()));
    if (deltaTime > Timer()) Die();
}

std::chrono::time_point<std::chrono::high_resolution_clock> Bomb::SpawnTime() const
{
    return _spawnTime;
}

std::chrono::duration<double> Bomb::Timer() const
{
    return _timer;
}

void Bomb::SetTimer(std::chrono::duration<double> timer)
{
    _timer = timer;
}

int Bomb::Range() const
{
    return _range;
}

void Bomb::SetRange(int range)
{
    _range = range;
}
