#pragma once

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

template <uint WorkGroupSizeX, uint WorkGroupSizeY, uint WorkGroupSizeZ>
inline CPUCompute<WorkGroupSizeX, WorkGroupSizeY, WorkGroupSizeZ>::CPUCompute()
{
}

template <uint WorkGroupSizeX, uint WorkGroupSizeY, uint WorkGroupSizeZ>
inline void CPUCompute<WorkGroupSizeX, WorkGroupSizeY, WorkGroupSizeZ>::Wait()
{
    _threads.PushCommand({}, true);
}

template <uint WorkGroupSizeX, uint WorkGroupSizeY, uint WorkGroupSizeZ>
template <typename Op, typename Shared>
inline void CPUCompute<WorkGroupSizeX, WorkGroupSizeY, WorkGroupSizeZ>::Dispatch(const glm::uvec3& a_NumGroups)
{
    ComputeInputs inputs {
        .numWorkGroups = a_NumGroups
    };
    for (auto x = 0u; x < inputs.numWorkGroups.x; ++x) {
        for (auto y = 0u; y < inputs.numWorkGroups.y; ++y) {
            for (auto z = 0u; z < inputs.numWorkGroups.z; ++z) {
                inputs.workGroupID = { x, y, z };
                _threads.Enqueue([this, inputs] {
                    if constexpr (std::is_same_v<Shared, void>)
                        _DispatchLocal<Op>(inputs);
                    else
                        _DispatchLocal<Op, Shared>(inputs);
                });
            }
        }
    }
}

template <uint WorkGroupSizeX, uint WorkGroupSizeY, uint WorkGroupSizeZ>
template <typename Op>
inline void CPUCompute<WorkGroupSizeX, WorkGroupSizeY, WorkGroupSizeZ>::_DispatchLocal(ComputeInputs& a_Inputs) const
{
    a_Inputs.workGroupSize = { WorkGroupSizeX, WorkGroupSizeY, WorkGroupSizeZ };
    for (auto x = 0u; x < WorkGroupSizeX; ++x) {
        for (auto y = 0u; y < WorkGroupSizeY; ++y) {
            for (auto z = 0u; z < WorkGroupSizeZ; ++z) {
                a_Inputs.localInvocationID    = { x, y, z };
                a_Inputs.globalInvocationID   = a_Inputs.workGroupID * a_Inputs.workGroupSize + a_Inputs.localInvocationID;
                a_Inputs.localInvocationIndex = a_Inputs.localInvocationID.z * a_Inputs.workGroupSize.x * a_Inputs.workGroupSize.y + a_Inputs.localInvocationID.y * a_Inputs.workGroupSize.x + a_Inputs.localInvocationID.x;
                Op {}(a_Inputs);
            }
        }
    }
}

template <uint WorkGroupSizeX, uint WorkGroupSizeY, uint WorkGroupSizeZ>
template <typename Op, typename Shared>
inline void CPUCompute<WorkGroupSizeX, WorkGroupSizeY, WorkGroupSizeZ>::_DispatchLocal(ComputeInputs& a_Inputs) const
{
    a_Inputs.workGroupSize = { WorkGroupSizeX, WorkGroupSizeY, WorkGroupSizeZ };
    Shared shared;
    for (auto x = 0u; x < WorkGroupSizeX; ++x)
        _workgroupSize
        {
            for (auto y = 0u; y < WorkGroupSizeY; ++y) {
                for (auto z = 0u; z < WorkGroupSizeZ; ++z) {
                    a_Inputs.localInvocationID    = { x, y, z };
                    a_Inputs.globalInvocationID   = a_Inputs.workGroupID * a_Inputs.workGroupSize + a_Inputs.localInvocationID;
                    a_Inputs.localInvocationIndex = a_Inputs.localInvocationID.z * a_Inputs.workGroupSize.x * a_Inputs.workGroupSize.y + a_Inputs.localInvocationID.y * a_Inputs.workGroupSize.x + a_Inputs.localInvocationID.x;
                    Op {}(a_Inputs, shared);
                }
            }
        }
}
}