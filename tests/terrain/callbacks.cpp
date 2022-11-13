/*
* @Author: gpinchon
* @Date:   2019-03-26 13:04:37
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:01
*/

#include "Common.hpp"
#include "Events.hpp" // for Events
#include "Node.hpp" // for Node
#include "Renderable.hpp" // for Renderable
#include "glm/glm.hpp" // for s_vec3, s_vec2, glm::vec3, glm::clamp
#include "scop.hpp" // for DOWNK, LEFTK, MouseMoveCallback
#include <Config.hpp>
#include <Engine.hpp> // for Stop
#include <Environment.hpp> // for Environment
#include <FPSCamera.hpp> // for FPSCamera
#include <GameController.hpp> // for Controller, GameController
#include <Keyboard.hpp> // for Keyboard
#include <Mesh.hpp> // for Mesh
#include <Mouse.hpp> // for Mouse
#include <Render.hpp> // for InternalQuality, SetInternalQua...
#include <SDL2/SDL_events.h> // for SDL_KeyboardEvent, SDL_Controll...
#include <SDL2/SDL_gamecontroller.h> // for SDL_CONTROLLER_AXIS_LEFTX, SDL_...
#include <SDL2/SDL_scancode.h> // for SDL_SCANCODE_KP_MINUS, SDL_SCAN...
#include <Tools.hpp>
#include <Window.hpp> // for Window
#include <csignal> // for raise, SIGSEGV
#include <iostream> // for operator<<, endl, basic_ostream
#include <math.h> // for M_PI
#include <memory> // for shared_ptr, dynamic_pointer_cast

static auto cameraRotation = glm::vec3(M_PI / 2.f, M_PI / 2.f, 5.f);
bool orbit = false;

void callback_camera(SDL_Event*)
{
    auto controller = GameController::Get(0);
    glm::vec2 raxis = glm::vec2(0, 0);
    glm::vec2 laxis = glm::vec2(0, 0);
    //float ltrigger = 0;
    //float rtrigger = 0;
    if (controller->is_connected()) {
        raxis.x = -controller->axis(SDL_CONTROLLER_AXIS_RIGHTX);
        raxis.y = -controller->axis(SDL_CONTROLLER_AXIS_RIGHTY);
        laxis.x = -controller->axis(SDL_CONTROLLER_AXIS_LEFTX);
        laxis.y = -controller->axis(SDL_CONTROLLER_AXIS_LEFTY);
        //ltrigger = controller->axis(SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        //rtrigger = controller->axis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
    } else {
        raxis.x = Keyboard::key(LEFTK) - Keyboard::key(RIGHTK);
        raxis.y = Keyboard::key(DOWNK) - Keyboard::key(UPK);
        laxis.x = 0;
        laxis.y = Keyboard::key(ZOOMK) - Keyboard::key(UNZOOMK);
        //ltrigger = Keyboard::key(SDL_SCANCODE_PAGEDOWN);
        //rtrigger = Keyboard::key(SDL_SCANCODE_PAGEUP);
    }
    //static glm::vec3  val = (glm::vec3){M_PI / 2.f, M_PI / 2.f, 5.f};
    cameraRotation.x += raxis.y * Events::delta_time() * Config::Get("LookSensitivity", 2.f);
    cameraRotation.y += raxis.x * Events::delta_time() * Config::Get("LookSensitivity", 2.f);
    cameraRotation.z -= laxis.y * Events::delta_time();

    //if (orbit) {
    //    Mouse::set_relative(SDL_FALSE);
    //    auto camera = std::dynamic_pointer_cast<FPSCamera>(FPSCamera::current());
    //    auto t = camera->target();
    //    cameraRotation.x = glm::clamp(cameraRotation.x, 0.01f, float(M_PI - 0.01f));
    //    cameraRotation.y = CYCLE(cameraRotation.y, 0, 2 * M_PI);
    //    cameraRotation.z = glm::clamp(cameraRotation.z, 0.1f, 1000.f);
    //    t->Position().y += rtrigger * Events::delta_time();
    //    t->Position().y -= ltrigger * Events::delta_time();
    //    camera->orbite(cameraRotation.x, cameraRotation.y, cameraRotation.z);
    //} else {
    //}
    Mouse::set_relative(SDL_TRUE);
    auto camera = std::dynamic_pointer_cast<FPSCamera>(FPSCamera::GetByName("main_camera"));
    camera->SetYaw(cameraRotation.x);
    camera->SetPitch(cameraRotation.y);
    camera->SetPosition(camera->Position() + float(raxis.x * Events::delta_time()) * camera->Right());
    camera->SetPosition(camera->Position() - float(raxis.y * Events::delta_time()) * camera->Forward());
    //FPSCameraUpdate();
}

void callback_scale(SDL_KeyboardEvent* event)
{
    static float scale = 1;

    if (event == nullptr || event->type == SDL_KEYUP) {
        return;
    }
    auto mesh = Mesh::GetByName("terrain_test");
    if (Keyboard::key(SDL_SCANCODE_LCTRL) == 0u) {
        return;
    }
    if (Keyboard::key(SDL_SCANCODE_KP_PLUS) != 0u) {
        scale += (0.005 * (Keyboard::key(SDL_SCANCODE_LSHIFT) + 1));
    } else if (Keyboard::key(SDL_SCANCODE_KP_MINUS) != 0u) {
        scale -= (0.005 * (Keyboard::key(SDL_SCANCODE_LSHIFT) + 1));
    }
    scale = glm::clamp(scale, 0.0001f, 1000.f);
    mesh->SetScale(glm::vec3(scale));
}

void switch_background()
{
    static int b = 0;
    b++;
    auto env = Environment::Get(b);
    if (env != nullptr) {
        Environment::set_current(env);
    } else {
        b = 0;
        env = Environment::Get(b);
        Environment::set_current(env);
    }
}

void callback_background(SDL_KeyboardEvent* event)
{
    if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u))) {
        return;
    }
    switch_background();
}

void controller_callback_background(SDL_ControllerButtonEvent* event)
{
    if (event == nullptr || (event->type != SDL_CONTROLLERBUTTONDOWN && event->type != SDL_JOYBUTTONDOWN)) {
        return;
    }
    switch_background();
}

void controller_callback_quality(SDL_ControllerButtonEvent* event)
{
    if (event == nullptr || (event->type != SDL_CONTROLLERBUTTONDOWN && event->type != SDL_JOYBUTTONDOWN)) {
        return;
    }
    GameController::Get(0)->rumble(0.5, 100);
    Render::SetInternalQuality(CYCLE(Render::InternalQuality() + 0.25, 0.5, 1.5));
}

void callback_quality(SDL_KeyboardEvent* event)
{
    if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u))) {
        return;
    }
    Render::SetInternalQuality(CYCLE(Render::InternalQuality() + 0.25, 0.5, 1.5));
}

bool rotate_model = true;

void controller_callback_rotation(SDL_ControllerButtonEvent* event)
{
    if (event == nullptr || (event->type != SDL_CONTROLLERBUTTONDOWN && event->type != SDL_JOYBUTTONDOWN)) {
        return;
    }
    rotate_model = !rotate_model;
}

void keyboard_callback_rotation(SDL_KeyboardEvent* event)
{
    if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u))) {
        return;
    }
    rotate_model = !rotate_model;
}

void callback_refresh(SDL_Event* /*unused*/)
{
    auto mesh = Renderable::Get(0);
    if (mesh == nullptr) {
        return;
    }
    if (rotate_model) {
        static float rotation = 0;
        rotation += 0.2 * Events::delta_time();
        rotation = CYCLE(rotation, 0, 2 * M_PI);
        mesh->SetRotation(glm::vec3(0, rotation, 0));
    }
    callback_camera(nullptr);
}

void callback_exit(SDL_KeyboardEvent* /*unused*/)
{
    Engine::Stop();
}

void callback_fullscreen(SDL_KeyboardEvent* event)
{
    if ((Keyboard::key(SDL_SCANCODE_RETURN) != 0u) && (Keyboard::key(SDL_SCANCODE_LALT) != 0u)) {
        static bool fullscreen = false;
        fullscreen = !fullscreen;
        Window::fullscreen(fullscreen);
    }
    (void)event;
}

void MouseWheelCallback(SDL_MouseWheelEvent* event)
{
    static auto camera = std::dynamic_pointer_cast<FPSCamera>(FPSCamera::GetByName("main_camera"));
    camera->SetFov(event->y * 0.01);
    camera->SetFov(glm::clamp(camera->Fov(), 1.0f, 45.f));
}

void MouseMoveCallback(SDL_MouseMotionEvent* event)
{
    //if (orbit) {
    //    static auto camera = std::dynamic_pointer_cast<FPSCamera>(FPSCamera::GetByName("main_camera"));
    //    if (Mouse::button(1)) {
    //        cameraRotation.x += event->yrel * Events::delta_time();
    //        cameraRotation.y -= event->xrel * Events::delta_time();
    //        cameraRotation.x = glm::clamp(cameraRotation.x, 0.01f, float(M_PI - 0.01f));
    //        cameraRotation.y = CYCLE(cameraRotation.y, 0, 2 * M_PI);
    //        cameraRotation.z = glm::clamp(cameraRotation.z, 0.01f, 1000.f);
    //    }
    //    camera->orbite(cameraRotation.x, cameraRotation.y, cameraRotation.z);
    //} else {
    //}
    static auto camera = std::dynamic_pointer_cast<FPSCamera>(FPSCamera::GetByName("main_camera"));
    cameraRotation.x += event->xrel * Events::delta_time();
    cameraRotation.y -= event->yrel * Events::delta_time();
    camera->SetYaw(cameraRotation.x);
    camera->SetPitch(cameraRotation.y);
}

void callback_crash(SDL_KeyboardEvent*)
{
    std::cout << "CRASH !!!" << std::endl;
    std::raise(SIGSEGV);
}

void setup_callbacks()
{
    Keyboard::set_callback(SDL_SCANCODE_KP_PLUS, callback_scale);
    Keyboard::set_callback(SDL_SCANCODE_KP_MINUS, callback_scale);
    Keyboard::set_callback(SDL_SCANCODE_SPACE, callback_background);
    Keyboard::set_callback(SDL_SCANCODE_ESCAPE, callback_exit);
    Keyboard::set_callback(SDL_SCANCODE_RETURN, callback_fullscreen);
    Keyboard::set_callback(SDL_SCANCODE_Q, callback_quality);
    Keyboard::set_callback(SDL_SCANCODE_R, keyboard_callback_rotation);
    Keyboard::set_callback(SDL_SCANCODE_C, callback_crash);
    //Mouse::set_relative(SDL_TRUE);
    Mouse::set_move_callback(MouseMoveCallback);
    Mouse::set_wheel_callback(MouseWheelCallback);
    Events::set_refresh_callback(callback_refresh);
    auto controller = GameController::Get(0);
    if (controller == nullptr)
        return;
    controller->set_button_callback(SDL_CONTROLLER_BUTTON_A, controller_callback_quality);
    controller->set_button_callback(SDL_CONTROLLER_BUTTON_B, controller_callback_rotation);
    controller->set_button_callback(SDL_CONTROLLER_BUTTON_Y, controller_callback_background);
}
