#include <MSG/Entity/FogArea.hpp>

namespace Msg::Entity::FogArea {
uint32_t& GetNbr()
{
    static auto s_Nbr = 0u;
    return s_Nbr;
}
}
