/*
* @Author: gpinchon
* @Date:   2020-08-16 16:10:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:54:57
*/

#include <Surface/Mesh.hpp>
#include <Surface/SphereMesh.hpp>
#include <Material/Material.hpp>
#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Engine.hpp>
#include <Animation/Animation.hpp>

#include "Flame.hpp"
#include "Game.hpp"
#include "Level.hpp"


Flame::Flame(Level& level)
    : GameEntity(level,  "Flame")
    , _spawnTime(std::chrono::high_resolution_clock::now())
{
    _size = glm::vec2(0.7);
}

Flame::~Flame()
{
    _updateSlot.Disconnect();
}

auto CreateFlameAsset()
{
    auto flameAsset{ std::make_shared<Assets::Asset>(Engine::GetResourcePath() / "models/fire.gltf") };
    Assets::Parser::AddParsingTask({
        Assets::Parser::ParsingTask::Type::Sync,
        flameAsset
    });
    auto meshes{ flameAsset->GetComponentsInChildren<Mesh>() };
    for (auto& mesh : meshes)
        mesh->Load();
    return meshes;
}

std::shared_ptr<Flame> Flame::Create(Level& level, const glm::ivec2& position)
{
    auto flame = std::make_shared<Flame>(level);
    static auto flameAsset = CreateFlameAsset();
    for (const auto& mesh : flameAsset)
        flame->AddSurface(mesh);
    flame->SetScale(glm::vec3(0.5));
    level.SetGameEntityPosition(position, flame);
    return flame;
}

std::chrono::time_point<std::chrono::high_resolution_clock> Flame::SpawnTime() const
{
    return _spawnTime;
}

std::chrono::duration<double> Flame::Timer() const
{
    return _timer;
}

void Flame::SetTimer(std::chrono::duration<double> timer)
{
    _timer = timer;
}

void Flame::Update(float delta)
{
    auto now = std::chrono::high_resolution_clock::now();
    if (now - SpawnTime() > Timer())
        _level.SetGameEntity(Position(), nullptr);
}