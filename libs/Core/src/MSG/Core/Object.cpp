#include <MSG/Core/Object.hpp>

#include <ostream>
#include <random>

namespace MSG::Core {
Object::Object()
{
    static auto s_Nbr { 0u };
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());
    _id   = dist(gen);
    _name = "Object_" + std::to_string(++s_Nbr);
}
}
