/*
* @Author: gpinchon
* @Date:   2019-03-27 13:38:46
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:59
*/

#pragma once

#include "Engine.hpp"
#include <SDL2/SDL.h>

const auto WIDTH = 1280;
const auto HEIGHT = 720;

/*
** Key binding
*/

#define DOWNK SDL_SCANCODE_DOWN
#define UPK SDL_SCANCODE_UP
#define LEFTK SDL_SCANCODE_LEFT
#define RIGHTK SDL_SCANCODE_RIGHT
#define ZOOMK SDL_SCANCODE_KP_PLUS
#define UNZOOMK SDL_SCANCODE_KP_MINUS

//bool			load_mtllib(const std::string &path);

/*
** Callback functions
*/
void setup_callbacks();
void callback_refresh(SDL_Event* event);
void callback_camera(SDL_Event* event);
void callback_scale(SDL_KeyboardEvent* event);
void callback_background(SDL_KeyboardEvent* event);
void callback_exit(SDL_KeyboardEvent* event);
void callback_fullscreen(SDL_KeyboardEvent* event);
void callback_quality(SDL_KeyboardEvent* event);
void controller_callback_background(SDL_ControllerButtonEvent* event);
void controller_callback_quality(SDL_ControllerButtonEvent* event);
void controller_callback_rotation(SDL_ControllerButtonEvent* event);
void keyboard_callback_rotation(SDL_KeyboardEvent* event);
void MouseWheelCallback(SDL_MouseWheelEvent* event);
void MouseMoveCallback(SDL_MouseMotionEvent* event);