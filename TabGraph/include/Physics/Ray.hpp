#pragma once

#include <glm/glm.hpp>

namespace TabGraph::Physics {
struct Ray
{
	glm::vec3 direction;
	glm::vec3 origin;
};
}
