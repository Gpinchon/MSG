/*
* @Author: gpinchon
* @Date:   2020-08-08 22:47:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:51
*/

#include <Animation/Animation.hpp>
#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Cameras/Camera.hpp>
#include <Engine.hpp>
#include <Events/InputDevice/Keyboard.hpp>
#include <Material/Material.hpp>
#include <Scene/Scene.hpp>
#include <Surface/Mesh.hpp>

#include "Bomb.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include "Player.hpp"

#define DOWNK Keyboard::Key::Down
#define UPK Keyboard::Key::Up
#define LEFTK Keyboard::Key::Left
#define RIGHTK Keyboard::Key::Right
#define ZOOMK Keyboard::Key::NumpadPlus
#define DROPK Keyboard::Key::Space
#define UNZOOMK Keyboard::Key::NumpadMinus

Player::Player(Level& level, const std::string& name, const glm::vec3& color)
    : GameEntity(level, name, "Player")
{
    _height = 0;
    Keyboard::OnKeyDown(DROPK).ConnectMember(this, &Player::DropBomb);
}

auto CreatePlayerAsset()
{
    auto playerAsset = std::make_shared<Assets::Asset>(Engine::GetResourcePath() / "models/bomberman.gltf");
    Assets::Parser::AddParsingTask({ Assets::Parser::ParsingTask::Type::Sync,
        playerAsset });
    playerAsset->GetComponent<Scene>()->GetComponent<Node>()->SetScale(glm::vec3(0.01f));
    return playerAsset;
}

std::shared_ptr<Player> Player::Create(Level& level, const glm::vec3& color)
{
    auto player = std::make_shared<Player>(level, "Player1", glm::vec3(1));
    static auto playerAsset = CreatePlayerAsset();
    auto playerAssetClone = playerAsset->GetComponent<Scene>()->Clone();
    auto playerNode = playerAssetClone->GetComponent<Node>();
    player->AddChild(playerNode);
    for (auto& animation : playerAssetClone->GetComponents<Animation>())
        player->AddAnimation(animation);
    player->PlayAnimation("idle", true);
    for (auto i = 0; i < player->GetSurfaceNbr(); ++i) {
        player->GetSurface(i)->GetComponentInChildrenByName<Material>("White")->SetDiffuse(color);
    }

    return player;
}

float Player::Speed() const
{
    return _speed;
}

void Player::SetSpeed(float speed)
{
    _speed = speed;
}

void Player::DropBomb(const Event::Keyboard& event) const
{
    if (event.state && !event.repeat && _level.GetGameEntity(Position()) == nullptr) {
        std::cout << Position().x << ' ' << Position().y << std::endl;
        Bomb::Create(_level, Position());
    } else if (_level.GetGameEntity(Position()) != nullptr)
        std::cout << _level.GetGameEntity(Position())->Type() << std::endl;
}

template <typename T>
auto ProjectPointOnPlane(const T& point, const T& planePosition, const T& planeNormal)
{
    auto pointToPlanePosition = point - planePosition;
    auto dist = dot(planeNormal, pointToPlanePosition);
    return point - dist * planeNormal;
}

struct Contact {
    glm::vec2 normal;
    float penetration;
};

auto AABBvsCircle(Contact& c, const glm::vec2& sphereCenter, float sphereRadius, const glm::vec2& boxPos, const glm::vec2& boxSize)
{
    auto& C = sphereCenter;
    auto& B = boxPos;
    auto BC = C - B; //D
    auto P = B + glm::clamp(BC, -boxSize, boxSize);
    auto CP = P - C;
    auto CPLen = length(CP);
    auto CPdBC = dot(CP, BC);
    if (CPLen < sphereRadius) {
        if (CPLen == 0.f)
            c.normal = -normalize(BC);
        else
            c.normal = CP / CPLen;
        c.penetration = sphereRadius - CPLen;
        return true;
    }
    return false;
}

void Player::Move(const glm::vec2& direction, float delta)
{
    auto size = 0.5f;
    auto dirLength = length(direction);
    if (dirLength == 0.f)
        return;
    auto dir = direction / dirLength;
    auto axis = dir * Speed() * delta;
    auto newPlayerPosition = Position() + axis;
    LookAt(Position() - direction);
    SetPosition(newPlayerPosition);
    PlayAnimation("run", true);
}

void Player::Die()
{
    _lives--;
    if (_animations.at("death").lock()->Playing()) {
        std::cout << "Already DED" << std::endl;
    }
    PlayAnimation("death", false);
}

void Player::Update(float delta)
{
    if (_animations.at("death").lock()->Playing())
        return;
    glm::vec2 input {};
    input.x = Keyboard::GetKeyState(UPK) - Keyboard::GetKeyState(DOWNK);
    input.y = Keyboard::GetKeyState(RIGHTK) - Keyboard::GetKeyState(LEFTK);
    auto inputLength = length(input);
    if (inputLength > 0.f) {
        input /= inputLength;
        auto cameraT = _level.CurrentCamera();
        auto cameraPosition = glm::vec2(cameraT->WorldPosition().x, cameraT->WorldPosition().z);
        auto cameraForward = normalize(glm::vec2(cameraT->Forward().x, cameraT->Forward().z));
        auto projPlayerPosition = ProjectPointOnPlane(Position(), cameraPosition, cameraForward);
        auto forward = normalize(Position() - projPlayerPosition);
        auto right = normalize(glm::vec2(cameraT->Right().x, cameraT->Right().z));
        Move(input * (forward + right), delta);
    } else
        PlayAnimation("idle", true);
    bool collides { false };
    do {
        collides = false;
        auto maxX = glm::clamp(int(Position().x + 1), 0, _level.Size().x - 1);
        auto minX = glm::clamp(int(Position().x - 1), 0, _level.Size().x - 1);
        auto maxY = glm::clamp(int(Position().y + 1), 0, _level.Size().y - 1);
        auto minY = glm::clamp(int(Position().y - 1), 0, _level.Size().y - 1);
        for (auto x = minX; x <= maxX; ++x) {
            for (auto y = minY; y <= maxY; ++y) {
                auto entity = _level.GetGameEntity(glm::ivec2(x, y));
                if (entity == nullptr) {
                    continue;
                }
                if (x == int(Position().x) && y == int(Position().y) && entity->Type() == "Bomb")
                    continue;
                Contact contact;
                auto intersects = AABBvsCircle(contact, Position(), 0.3f, entity->Position(), entity->Size() * 0.5f);
                if (intersects) {
                    if (entity->Type() == "Flame") {
                        Die();
                        return;
                    }
                    SetPosition(Position() - contact.normal * std::max(contact.penetration, 0.001f));
                    collides = true;
                }
            }
        }
    } while (collides);
}