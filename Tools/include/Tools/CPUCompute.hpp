#ifndef CPU_COMPUTE_HPP
#define CPU_COMPUTE_HPP
#include <Tools/ThreadPool.hpp>

#include <functional>

#include <glm/vec3.hpp>

namespace TabGraph::Tools {
struct ComputeInputs {
    glm::uvec3 numWorkGroups;
    glm::uvec3 workGroupSize;
    glm::uvec3 workGroupID;
    glm::uvec3 localInvocationID;
    glm::uvec3 globalInvocationID;
    uint localInvocationIndex;
};

template <uint WorkGroupSizeX, uint WorkGroupSizeY, uint WorkGroupSizeZ>
class CPUCompute {
public:
    explicit CPUCompute();
    template <typename Op, typename Shared = void>
    void Dispatch(const glm::uvec3& a_NumGroups);
    void Wait();

private:
    template <typename Op, typename Shared>
    void _DispatchLocal(ComputeInputs& a_Inputs) const;
    template <typename Op>
    void _DispatchLocal(ComputeInputs& a_Inputs) const;
    ThreadPool _threads;
};

#include <Tools/CPUCompute.inl>

#endif // CPU_COMPUTE_HPP