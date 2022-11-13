/*
* @Author: gpinchon
* @Date:   2020-08-08 19:50:33
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-13 23:42:47
*/

#define USE_HIGH_PERFORMANCE_GPU
#include <DLLExport.hpp>
#include <Engine.hpp>

#include "Config.hpp"
#include "Game.hpp"
#include "Level.hpp"

int main(int /*argc*/, char const** /*argv*/)
{
    #ifndef _WIN32
    //this is to make terminate signal crash on Linux
    std::set_terminate([]() { std::cout << "Unhandled exception\n"; int* p = nullptr; p[0] = 1; });
    #endif
    Config::Global().Parse(Engine::GetResourcePath() / "config.ini");
    Game::Init();
    Game::AddPlayer();
    auto level = Level::Create(Engine::GetResourcePath() / "maps/test.map");
    Game::SetCurrentLevel(level);
    Game::Start();
    return 0;
}