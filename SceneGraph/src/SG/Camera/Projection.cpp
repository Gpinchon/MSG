/*
* @Author: gpinchon
* @Date:   2021-07-21 21:58:38
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-21 22:04:41
*/

#include <SG/Camera/Projection.hpp>

#include <Tools/Halton.hpp>

#include <glm/vec2.hpp>


namespace TabGraph::SG {
void CameraProjection::_ApplyTemporalJitter(glm::mat4& a_Matrix, uint8_t a_Index, float a_Intensity)
{
    const auto halton2 = (Tools::HaltonSequence<2, 256>(a_Index) * 2.0 - 1.0) * 0.25f;
    const auto halton3 = (Tools::HaltonSequence<3, 256>(a_Index) * 2.0 - 1.0) * 0.25f;
    a_Matrix[2][0] += halton2 * a_Intensity;
    a_Matrix[2][1] += halton3 * a_Intensity;
}
}
