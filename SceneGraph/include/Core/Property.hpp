#pragma once

/** Use this to declare a new property */
#define PROPERTY(type, var, ...)   \
public:                            \
    type Get##var() const          \
    {                              \
        return _##var;             \
    }                              \
    void Set##var(const type& val) \
    {                              \
        _##var = val;              \
    }                              \
                                   \
private:                           \
    type _##var { __VA_ARGS__ };

#define READONLYPROPERTY(type, var, ...) \
public:                             \
    type Get##var() const           \
    {                               \
        return _##var;              \
    }                               \
                                    \
protected:                          \
    void _Set##var(const type& val) \
    {                               \
        _##var = val;               \
    }                               \
private:                            \
    type _##var { __VA_ARGS__ };

#define PRIVATEPROPERTY(type, var, ...) \
private:                                \
    type _Get##var() const              \
    {                                   \
        return _##var;                  \
    }                                   \
    void _Set##var(const type& val)     \
    {                                   \
        _##var = val;                   \
    }                                   \
    type _##var { __VA_ARGS__ };