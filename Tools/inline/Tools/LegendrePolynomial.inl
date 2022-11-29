#include <Tools/Pi.hpp>
#include <Tools/Factorial.hpp>
#include <gcem.hpp>


namespace TabGraph::Tools {
constexpr double LegendrePolynomial(int32_t l, int32_t m, double x)
{
    //Legendre Polynomial at X
    //@ref https://en.wikipedia.org/wiki/Associated_Legendre_polynomials
    double pmm{ 1.0 };
    if (m > 0) {
        double sign = (m % 2 == 0 ? 1 : -1);
        if (__builtin_is_constant_evaluated())
            pmm = sign * DoubleFactorial(2 * m - 1) * gcem::pow(1 - x * x, m / 2.0);
        else pmm = sign * DoubleFactorial(2 * m - 1) * std::pow(1 - x * x, m / 2.0);
    }
    if (l == m)
        return pmm;

    double pmm1{ x * (2 * m + 1) * pmm };
    if (l == m + 1)
        return pmm1;
    for (int n = m + 2; n <= l; n++) {
        double pmn = (x * (2 * n - 1) * pmm1 - (n + m - 1) * pmm) / (n - m);
        pmm = pmm1;
        pmm1 = pmn;
    }
    return pmm1;
}

template<typename T>
constexpr auto feq(const T x, const T y, const T tol = std::numeric_limits<T>::epsilon()) {
    return gcem::abs(x - y) <= tol;
}

static_assert(feq(LegendrePolynomial(2, 1, -1),    0.0));
static_assert(feq(LegendrePolynomial(2, 1, -0.96), 0.8064));
}
