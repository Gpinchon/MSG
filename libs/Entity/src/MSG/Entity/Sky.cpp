#include <MSG/Entity/Sky.hpp>

#include <cstdint>

uint32_t& MSG::Entity::LightSky::GetNbr()
{
    static auto s_Nbr = 0u;
    return s_Nbr;
}
