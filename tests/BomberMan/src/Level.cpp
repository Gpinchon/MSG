/*
* @Author: gpinchon
* @Date:   2020-08-08 20:11:05
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:24
*/

#include "Assets/Asset.hpp"
#include "Assets/Image.hpp"
#include "Camera/OrbitCamera.hpp"
#include "Engine.hpp"
#include "Light/DirectionalLight.hpp"
#include "Light/HDRLight.hpp"
#include "Material/Material.hpp"
#include "Surface/Mesh.hpp"
#include "Surface/PlaneMesh.hpp"
#include "Surface/Skybox.hpp"
#include "Texture/Texture2D.hpp"
#include "Texture/TextureCubemap.hpp"
#include "Texture/Sampler.hpp"

#include "CrispyWall.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "Wall.hpp"

#include <fstream>
#include <sstream>

Level::Level(const std::string& name, const glm::ivec2& size)
    : Scene(name)
    , _size(size)
    , _entities(size.x * size.y)
    , _spawnPoint(glm::ivec2(0))
{

    /*
    for (auto x = 0; x < Size().x; ++x) {
        if (x != 0 && x != Size().x - 1)
            continue;
        for (auto y = 0; y < Size().y; ++y) {
            if (y != 0 && y != Size().y - 1)
                continue;
            auto wall = Wall::Create();
            wall->GetComponent<Transform>()->SetPosition(glm::vec3(x + 0.5, 0.5, y + 0.5));
            SetGameEntity(glm::ivec2(x, y), wall);
        }
    }*/
    //Add(cameraNode);
    //Engine::OnFixedUpdate().ConnectMember(this, &Level::_FixedUpdateCPU);
    //Engine::OnUpdate().ConnectMember(this, &Level::_UpdateCPU);
}

Level::~Level()
{
}

#include "Texture/Texture2D.hpp"

std::shared_ptr<Level> Level::Create(const std::filesystem::path& path)
{
    std::ifstream file;
    file.open(path);
    std::vector<std::vector<int>> map;
    while (file.good()) {
        std::string line;
        std::getline(file, line);
        if (line.find("//") == 0)
            continue;
        std::stringstream iss(line);
        int number;
        std::vector<int> lineNumbers;
        while (iss >> number) {
            lineNumbers.push_back(number);
        }
        map.push_back(lineNumbers);
    }
    int maxLine = 0;
    for (const auto& row : map) {
        if (row.size() > maxLine)
            maxLine = row.size();
    }
    auto size = glm::ivec2(map.size(), maxLine);
    auto level(std::make_shared<Level>(path.string(), size));
    auto floorMesh = PlaneMesh::Create("FloorMesh", level->Size());
    auto floorNode = std::make_shared<Node>("FloorNode");
    floorNode->SetPosition(glm::vec3(level->Size().x / 2.f, 0, level->Size().y / 2.f));
    //auto light = DirectionnalLight::Create("MainLight", glm::vec3(1.f), glm::vec3(1.f), 1.f, true);
    auto radius = size.x + size.y;
    auto light = std::make_shared<DirectionalLight>("MainLight", glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), true);
    light->SetHalfSize(glm::vec3(size.x / 2.f, 1.5, size.y / 2.f));
    light->SetPosition(glm::vec3(size.x / 2.f, 0, size.y / 2.f));
    light->SetInfinite(false);
    auto camera = std::make_shared<OrbitCamera>("MainCamera", glm::pi<float>() / 2.f, 1, radius / 2.f);
    camera->SetTarget(floorNode);
    floorNode->AddSurface(floorMesh);
    level->SetCurrentCamera(camera);
    level->Add(light);
    level->Add(floorNode);
    //auto textureData = std::vector<glm::u8vec4>(size_t(size.x) * size.y);
    auto floorImage = std::make_shared<Assets::Image>(size, Pixel::SizedFormat::Uint8_NormalizedRGBA);
    for (auto y = 0; y < size.y; ++y) {
        for (auto x = 0; x < size.x; ++x) {
            auto index = x + y * size.x;
            auto color1 = glm::vec4(0.133333, 0.270588, 0.000000, 1);
            auto color0 = glm::vec4(0.192156, 0.388235, 0.000000, 1);
            auto color{ index % 2 ? color1 : color0 };
            floorImage->SetColor(glm::ivec2(x, y), color);
        }
    }
    for (auto x = 0u; x < map.size(); ++x) {
        for (auto y = 0u; y < map.at(x).size(); ++y) {
            auto index = x + y * size.x;
            switch (map.at(x).at(y)) {
            case 1: {
                auto wall = Wall::Create(*level.get());
                level->SetGameEntityPosition(glm::ivec2(x, y), wall);
                auto color{ glm::vec4(0.274509, 0.050980, 0.050980, 1) };
                floorImage->SetColor(glm::ivec2(x, y), color);
                break;
            }
            case 2: {
                level->SetSpawnPoint(glm::ivec2(x, y));
                break;
            }
            case 3: {
                auto wall = CrispyWall::Create(*level.get());
                level->SetGameEntityPosition(glm::ivec2(x, y), wall);
            }
            }
        }
    }
    auto floorImageAsset{ std::make_shared<Assets::Asset>() };
    floorImageAsset->AddComponent(floorImage);
    floorImageAsset->SetLoaded(true);
    auto floorTexture = std::make_shared<Textures::Texture2D>(floorImageAsset);
    floorTexture->GetSampler()->SetMagFilter(Textures::Sampler::Filter::Nearest);
    floorTexture->GetSampler()->SetMinFilter(Textures::Sampler::Filter::Nearest);
    floorMesh->GetGeometryMaterial(0)->SetTextureDiffuse(floorTexture);
    auto skybox = std::make_shared<Skybox>("Skybox");
    auto diffuseMap{ std::make_shared<Assets::Asset>(path.parent_path() / "env.png") };
    skybox->SetTexture(std::make_shared<TextureCubemap>(diffuseMap));
    level->SetSkybox(skybox);

    auto lightHDR{ std::make_shared<HDRLight>(diffuseMap) };
    level->Add(lightHDR);
    for (auto i = 0u; i < Game::PlayerNumber(); ++i) {
        auto player{ Player::Create(*level.get(), glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f)) };
        player->SetPosition(level->SpawnPoint());
        level->Add(player);
        level->_players.push_back(player);
    }
    return level;
}

void Level::SetSpawnPoint(glm::ivec2 spawnPoint)
{
    _spawnPoint = spawnPoint;
}

glm::ivec2 Level::SpawnPoint() const
{
    return _spawnPoint;
}

glm::ivec2 Level::Size() const
{
    return _size;
}

void Level::SetGameEntityPosition(glm::ivec2 position, std::shared_ptr<GameEntity> entity)
{
    entity->SetPosition(glm::vec2(position) + entity->Size() / 2.f);
    SetGameEntity(position, entity);
}

void Level::SetGameEntity(glm::ivec2 position, std::shared_ptr<GameEntity> entity)
{
    position = glm::clamp(position, glm::ivec2(0), Size() - 1);
    auto index { position.x + position.y * Size().x };
    Remove(_entities.at(index).lock());
    _entities.at(index).lock().reset();
    _entities.at(index).reset();
    if (entity != nullptr) {
        _entities.at(index) = entity;
        Add(entity);
    }
}

std::shared_ptr<GameEntity> Level::GetGameEntity(glm::ivec2 position) const
{
    return _entities.at(position.x + position.y * Size().x).lock();
}

void Level::Update(float delta)
{
    for (auto& entity : _entities) {
        if (entity.lock() != nullptr)
            entity.lock()->Update(delta);
    }
    for (auto& player : _players) {
        if (player.lock() != nullptr)
            player.lock()->Update(delta);
    }
}
