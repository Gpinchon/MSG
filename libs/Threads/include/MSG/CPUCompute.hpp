#ifndef CPU_COMPUTE_HPP
#define CPU_COMPUTE_HPP
#include <MSG/ThreadPool.hpp>

#include <functional>

#include <glm/vec3.hpp>

namespace MSG {
struct CPUComputeInputs {
    glm::uvec3 numWorkGroups;
    glm::uvec3 workGroupSize;
    glm::uvec3 workGroupID;
    glm::uvec3 localInvocationID;
    glm::uvec3 globalInvocationID;
    uint32_t localInvocationIndex;
};

/**
 * @brief this class is designed to mimic OGL compute shaders as close as possible
 */
template <uint32_t WorkGroupSizeX = 1, uint32_t WorkGroupSizeY = 1, uint32_t WorkGroupSizeZ = 1>
class CPUCompute {
public:
    template <typename Op, typename Shared = void>
    void Dispatch(Op& a_Op, const glm::uvec3& a_NumGroups);
    void Wait();

private:
    template <typename Op, typename Shared>
    void _DispatchLocal(Op& a_Op, CPUComputeInputs& a_Inputs) const;
    template <typename Op>
    void _DispatchLocal(Op& a_Op, CPUComputeInputs& a_Inputs) const;
    ThreadPool _threads;
};
}

#include <MSG/CPUCompute.inl>

#endif // CPU_COMPUTE_HPP
