#include <MSG/Entity/Camera.hpp>

namespace MSG::Entity::Camera {
uint32_t& GetNbr()
{
    static auto s_Nbr = 0u;
    return s_Nbr;
}
}
