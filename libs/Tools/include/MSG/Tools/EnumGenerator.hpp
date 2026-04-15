#pragma once

// Source - https://github.com/swansontec/map-macro/
// by William Swanson
#define EVAL0(...) __VA_ARGS__
#define EVAL1(...) EVAL0(EVAL0(EVAL0(__VA_ARGS__)))
#define EVAL2(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL3(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL4(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL(...)  EVAL4(__VA_ARGS__)
#define NOP
#define MAP_POP0(F, X, ...) F(X) __VA_OPT__(MAP_POP1 NOP(F, __VA_ARGS__))
#define MAP_POP1(F, X, ...) F(X) __VA_OPT__(MAP_POP0 NOP(F, __VA_ARGS__))
#define MAP(F, ...)         __VA_OPT__(EVAL(MAP_POP0(F, __VA_ARGS__)))

#define TO_STRING(ARG)           #ARG
#define _ENUM_VAL_ADD_COMMA(ARG) TO_STRING(ARG),
#define GEN_ENUM_TO_STRING(NAME, TYPE, ...)              \
    inline char* const NAME##ToString(const TYPE& a_Val) \
    {                                                    \
        constexpr char* s_Strings[] = {                  \
            __VA_ARGS__                                  \
        };                                               \
        return s_Strings[a_Val];                         \
    }
#define GEN_ENUM_TO_STRING1(NAME, TYPE, ...)             \
    inline char* const NAME##ToString(const NAME& a_Val) \
    {                                                    \
        return NAME##ToString(TYPE(a_Val));              \
    }
#define GEN_ENUM(NAME, TYPE, ...) \
    enum NAME : TYPE {            \
        __VA_ARGS__               \
    };
#define DECLARE_ENUM(NAME, ...)                                               \
    GEN_ENUM(NAME, uint32_t, __VA_ARGS__)                                     \
    GEN_ENUM_TO_STRING(NAME, uint32_t, MAP(_ENUM_VAL_ADD_COMMA, __VA_ARGS__)) \
    GEN_ENUM_TO_STRING1(NAME, uint32_t, MAP(_ENUM_VAL_ADD_COMMA, __VA_ARGS__))
