#pragma once

#include <utility>

namespace Msg::Tools {
template <class Name, typename Type>
class StrongTypedef {
public:
    using type                = Type;
    constexpr StrongTypedef() = default;
    constexpr explicit StrongTypedef(const type& value)
        : _value(value)
    {
    }
    constexpr explicit StrongTypedef(type&& value) noexcept(std::is_nothrow_move_constructible<type>::value)
        : _value(std::move(value))
    {
    }
    explicit operator type&() noexcept { return _value; }
    explicit operator const type&() const noexcept { return _value; }
    friend void swap(StrongTypedef& a, StrongTypedef& b) noexcept
    {
        using std::swap;
        swap(static_cast<type&>(a), static_cast<type&>(b));
    }
    bool operator==(const StrongTypedef<Name, Type>& a_Rhs) const { return _value == a_Rhs._value; }

private:
    type _value;
};
}

#define MSG_STRONG_TYPEDEF(Name, Type)               \
    struct Name : Tools::StrongTypedef<Name, Type> { \
        using StrongTypedef::StrongTypedef;          \
    };
