#include <MSG/Core/Orientation.hpp>
#include <glm/vec3.hpp>

namespace MSG {
static glm::vec3 s_up(0, 1, 0);
static glm::vec3 s_forward(0, 0, -1);
static glm::vec3 s_right(1, 0, 0);
static glm::vec3 s_gravity(0, -9.81, 0);
}

const glm::vec3& MSG::Core::Up()
{
    return s_up;
}

void MSG::Core::SetUp(const glm::vec3& up)
{
    s_up = up;
}

const glm::vec3& MSG::Core::Forward()
{
    return s_forward;
}

void MSG::Core::SetForward(const glm::vec3& up)
{
    s_forward = up;
}

const glm::vec3& MSG::Core::Right()
{
    return s_right;
}

void MSG::Core::SetRight(const glm::vec3& up)
{
    s_right = up;
}

const glm::vec3& MSG::Core::Gravity()
{
    return s_gravity;
}

void MSG::Core::SetGravity(const glm::vec3& gravity)
{
    s_gravity = gravity;
}
