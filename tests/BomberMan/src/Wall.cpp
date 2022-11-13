/*
* @Author: gpinchon
* @Date:   2020-08-09 22:51:26
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 14:00:45
*/
#include <Material/Material.hpp>
#include <Surface/CubeMesh.hpp>
#include <Surface/Mesh.hpp>
#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Engine.hpp>
#include <Scene/Scene.hpp>

#include "Wall.hpp"

Wall::Wall(Level& level)
    : GameEntity(level, "Wall")
{
    
}

auto CreateWallAsset() {
    auto wallAsset{ std::make_shared<Assets::Asset>(Engine::GetResourcePath() / "models/wall.gltf") };
    Assets::Parser::AddParsingTask({
        Assets::Parser::ParsingTask::Type::Sync,
        wallAsset
    });
    return wallAsset->GetComponentsInChildren<Mesh>();
}

std::shared_ptr<Wall> Wall::Create(Level& level)
{
    static auto wallMeshes(CreateWallAsset());
    //auto wall{ std::static_pointer_cast<Wall>(wallAsset->Clone()) };
    auto wall(std::make_shared<Wall>(level));
    for (const auto& mesh : wallMeshes)
        wall->AddSurface(mesh);
    wall->SetScale(glm::vec3(0.45));
    return wall;
}