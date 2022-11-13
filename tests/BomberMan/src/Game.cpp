#include <Cameras/Camera.hpp>
#include <Engine.hpp>
#include <Renderer/Renderer.hpp>
#include <Window.hpp>

#include "Game.hpp"
#include "Level.hpp"
#include "Player.hpp"

#include <array>

struct GameManager {
    GameManager()
        : engine(Engine::Create(Renderer::FrameRenderer::Create(Window::Create("Bomberman", glm::ivec2(1280, 720)))))
    {
        engine->OnFixedUpdate().Connect(&Game::Update);
        engine->GetFrameRenderer()->GetWindow()->OnEvent(Event::Window::Type::SizeChanged).Connect(
            [engine = std::weak_ptr(engine)](const Event::Window& event) {
            if (engine.lock()->GetCurrentScene() == nullptr)
                return;
            auto scene{ engine.lock()->GetCurrentScene() };
            auto proj = scene->CurrentCamera()->GetProjection();
            if (proj.type == Camera::Projection::Type::PerspectiveInfinite) {
                auto perspectiveInfinite = proj.Get<Camera::Projection::PerspectiveInfinite>();
                perspectiveInfinite.aspectRatio = event.window->GetSize().x / float(event.window->GetSize().y);
                scene->CurrentCamera()->SetProjection(perspectiveInfinite);
            }
            else if (proj.type == Camera::Projection::Type::Perspective) {
                auto perspective = proj.Get<Camera::Projection::Perspective>();
                perspective.aspectRatio = event.window->GetSize().x / float(event.window->GetSize().y);
                scene->CurrentCamera()->SetProjection(perspective);
            }
        });
    }
    std::shared_ptr<Level> currentLevel;
    std::shared_ptr<Engine> engine;
    int playerNumber { 0 };
};

static std::unique_ptr<GameManager> s_gameManager;

void GameRefresh(float delta)
{
    Game::Update(delta);
}

GameManager& getGameManager()
{
    return *s_gameManager;
}

void Game::SetCurrentLevel(std::shared_ptr<Level> currentLevel)
{
    getGameManager().currentLevel = currentLevel;
    getGameManager().engine->SetCurrentScene(currentLevel);
}

std::shared_ptr<Level> Game::CurrentLevel()
{
    return getGameManager().currentLevel;
}

void Game::Update(float step)
{
    getGameManager().currentLevel->Update(step);
}

int Game::PlayerNumber()
{
    return getGameManager().playerNumber;
}

int Game::AddPlayer()
{
    if (getGameManager().playerNumber == 4)
        return -1;
    getGameManager().playerNumber++;
    return getGameManager().playerNumber - 1;
}

void Game::MovePlayer(int index, const glm::vec2& input)
{
}

void Game::Init()
{
    if (s_gameManager == nullptr)
        s_gameManager.reset(new GameManager);
}

void Game::Start()
{
    s_gameManager->engine->Start();
}
