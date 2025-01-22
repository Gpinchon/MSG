#pragma once

// When you want Pi but you don't want the (after) math

#ifndef M_PI
constexpr auto M_PI = 3.14159265358979323846;
#endif // M_PI
#ifndef M_PIf
constexpr auto M_PIf = float(M_PI);
#endif // M_PIf

#ifndef HALF_PI
constexpr auto HALF_PI = (M_PI * 0.5);
#endif // HALF_PI

#ifndef HALF_PIf
constexpr auto HALF_PIf = float(HALF_PI);
#endif // HALF_PIf
