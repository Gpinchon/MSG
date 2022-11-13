/*
* @Author: gpinchon
* @Date:   2020-08-27 18:48:19
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:42:39
*/

#include "Common.hpp"
#include <glm/vec3.hpp>

static glm::vec3 s_up(0, 1, 0);
static glm::vec3 s_forward(0, 0, -1);
static glm::vec3 s_right(1, 0, 0);
static glm::vec3 s_gravity(0, -9.81, 0);

glm::vec3 Common::Up()
{
    return s_up;
}

void Common::SetUp(glm::vec3 up)
{
    s_up = up;
}

glm::vec3 Common::Forward()
{
    return s_forward;
}

void Common::SetForward(glm::vec3 up)
{
    s_forward = up;
}

glm::vec3 Common::Right()
{
    return s_right;
}

void Common::SetRight(glm::vec3 up)
{
    s_right = up;
}

glm::vec3 Common::Gravity()
{
    return s_gravity;
}

void Common::SetGravity(glm::vec3 gravity)
{
    s_gravity = gravity;
}