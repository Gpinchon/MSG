#pragma once

#include <memory>
#include <type_traits>

#define CYCLE(nbr, min, max) (nbr >= max ? min : nbr <= min ? max \
                                                            : nbr)
#define BUFFER_OFFSET(i) (reinterpret_cast<const char*>(i))

/*
** Useful to secure for templates
** @arg T : the shared pointer type you want
** @arg U : the type to test
** @arg V : return type
*/
template <typename T, typename U, typename V = void>
using IsSharedPointerOfType = std::enable_if_t<std::is_constructible_v<std::shared_ptr<T>, std::shared_ptr<U>>, V>;

/*
** Useful to secure for templates
** @arg T : the base class
** @arg U : the type to test
** @arg V : return type
*/
template <typename T, typename U, typename V = void>
using IsBaseOf = std::enable_if_t<std::is_base_of_v<T, U>, V>;

/*
** Useful to secure for templates
** @arg T : the shared pointer type you want
** @arg U : the type to test
** @arg V : return type
*/
template <typename T, typename U, typename V = void>
using IsNotSharedPointerOfType = std::enable_if_t<!std::is_constructible_v<std::shared_ptr<T>, U>, V>;
