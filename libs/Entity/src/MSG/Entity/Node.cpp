#include <cstdint>

namespace Msg::Entity::Node {
uint32_t& GetNbr()
{
    static auto s_Nbr = 0u;
    return s_Nbr;
}
}
