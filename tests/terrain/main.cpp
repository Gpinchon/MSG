/*
* @Author: gpinchon
* @Date:   2020-08-19 23:13:52
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:00
*/
/*
 * @Author: gpinchon
 * @Date:   2019-03-27 13:38:46
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2019-08-11 12:18:07
 */

#define USE_HIGH_PERFORMANCE_GPU
#include "DLLExport.hpp"

#include "scop.hpp" // for setup_callbacks
#include <Config.hpp> // for Config
#include <Engine.hpp> // for ResourcePath, Init, Start
#include <FPSCamera.hpp> // for OrbitCamera, Camera
#include <Light.hpp> // for DirectionnalLight
#include <Mouse.hpp> // for Mouse
#include <Node.hpp> // for Node
#include <SDL2/SDL.h> // for SDL_Quit
#include <SDL2/SDL_main.h> // for main
#include <Terrain.hpp> // for Terrain
#include <glm/vec2.hpp> // for vec<>::vec<2, type-parameter-0-...
#include <glm/vec3.hpp> // for vec<>::vec<3, type-parameter-0-...
#include <math.h> // for M_PI
#include <memory> // for __shared_ptr_access, shared_ptr
#include <string> // for operator+, string

int main(int argc, char** argv)
{
    Config::Parse(Engine::ResourcePath() + "config.ini");
    Engine::Init();
    auto camera = FPSCamera::Create("main_camera", 45);
    Camera::set_current(camera);
    if (argc > 1)
        Terrain::Create("terrain_test", glm::vec2(1024, 1024), argv[1]);
    else
        Terrain::Create("terrain_test", glm::vec2(1024, 1024), Engine::ResourcePath() + "crater.bt");
    setup_callbacks();
    DirectionnalLight::Create("MainLight", glm::vec3(1, 1, 1), glm::vec3(10, 10, 10), 1, true);
    Engine::Start();
    SDL_Quit();
    return (0);
}