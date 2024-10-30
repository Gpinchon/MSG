#include <SG/POSymetricMatrix.hpp>

#include <glm/vec3.hpp>

namespace TabGraph::SG {
POSymetricMatrix::POSymetricMatrix(double c) { fill(c); }
POSymetricMatrix::POSymetricMatrix(double a, double b, double c, double d)
    : std::array<double, 10>(
          { a * a, a * b, a * c, a * d,
              b * b, b * c, b * d,
              c * c, c * d,
              d * d })
{
}
double POSymetricMatrix::Error(const glm::vec3& a_V) const
{
    auto& q = *this;
    return (q[0] * a_V.x * a_V.x) + (2 * q[1] * a_V.x * a_V.y) + (2 * q[2] * a_V.x * a_V.z) + (2 * q[3] * a_V.x)
        + (q[4] * a_V.y * a_V.y) + (2 * q[5] * a_V.y * a_V.z) + (2 * q[6] * a_V.y)
        + (q[7] * a_V.z * a_V.z) + (2 * q[8] * a_V.z)
        + (q[9]);
}
POSymetricMatrix& POSymetricMatrix::operator+=(const POSymetricMatrix& n)
{
    for (uint8_t i = 0; i < size(); ++i)
        at(i) += n.at(i);
    return *this;
}
POSymetricMatrix POSymetricMatrix::operator+(const POSymetricMatrix& n) const { return POSymetricMatrix { *this } += n; }
}
