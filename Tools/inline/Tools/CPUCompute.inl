#ifdef CPU_COMPUTE_HPP

namespace TabGraph::Tools {
template <uint WorkGroupSizeX, uint WorkGroupSizeY, uint WorkGroupSizeZ>
inline void CPUCompute<WorkGroupSizeX, WorkGroupSizeY, WorkGroupSizeZ>::Wait()
{
    _threads.Wait();
}

template <uint WorkGroupSizeX, uint WorkGroupSizeY, uint WorkGroupSizeZ>
template <typename Op, typename Shared>
inline void CPUCompute<WorkGroupSizeX, WorkGroupSizeY, WorkGroupSizeZ>::Dispatch(Op& a_Op, const glm::uvec3& a_NumGroups)
{
    ComputeInputs inputs {
        .numWorkGroups = a_NumGroups
    };
    for (auto x = 0u; x < inputs.numWorkGroups.x; ++x) {
        for (auto y = 0u; y < inputs.numWorkGroups.y; ++y) {
            for (auto z = 0u; z < inputs.numWorkGroups.z; ++z) {
                inputs.workGroupID = { x, y, z };
                _threads.Enqueue([this, a_Op, inputs = inputs]() mutable {
                    if constexpr (std::is_same_v<Shared, void>)
                        _DispatchLocal<Op>(a_Op, inputs);
                    else
                        _DispatchLocal<Op, Shared>(a_Op, inputs);
                });
            }
        }
    }
}

template <uint WorkGroupSizeX, uint WorkGroupSizeY, uint WorkGroupSizeZ>
template <typename Op>
inline void CPUCompute<WorkGroupSizeX, WorkGroupSizeY, WorkGroupSizeZ>::_DispatchLocal(Op& a_Op, ComputeInputs& a_Inputs) const
{
    a_Inputs.workGroupSize = { WorkGroupSizeX, WorkGroupSizeY, WorkGroupSizeZ };
    for (auto x = 0u; x < WorkGroupSizeX; ++x) {
        for (auto y = 0u; y < WorkGroupSizeY; ++y) {
            for (auto z = 0u; z < WorkGroupSizeZ; ++z) {
                a_Inputs.localInvocationID    = { x, y, z };
                a_Inputs.globalInvocationID   = a_Inputs.workGroupID * a_Inputs.workGroupSize + a_Inputs.localInvocationID;
                a_Inputs.localInvocationIndex = a_Inputs.localInvocationID.z * a_Inputs.workGroupSize.x * a_Inputs.workGroupSize.y + a_Inputs.localInvocationID.y * a_Inputs.workGroupSize.x + a_Inputs.localInvocationID.x;
                a_Op(a_Inputs);
            }
        }
    }
}

template <uint WorkGroupSizeX, uint WorkGroupSizeY, uint WorkGroupSizeZ>
template <typename Op, typename Shared>
inline void CPUCompute<WorkGroupSizeX, WorkGroupSizeY, WorkGroupSizeZ>::_DispatchLocal(Op& a_Op, ComputeInputs& a_Inputs) const
{
    a_Inputs.workGroupSize = { WorkGroupSizeX, WorkGroupSizeY, WorkGroupSizeZ };
    Shared shared;
    for (auto x = 0u; x < WorkGroupSizeX; ++x) {
        for (auto y = 0u; y < WorkGroupSizeY; ++y) {
            for (auto z = 0u; z < WorkGroupSizeZ; ++z) {
                a_Inputs.localInvocationID    = { x, y, z };
                a_Inputs.globalInvocationID   = a_Inputs.workGroupID * a_Inputs.workGroupSize + a_Inputs.localInvocationID;
                a_Inputs.localInvocationIndex = a_Inputs.localInvocationID.z * a_Inputs.workGroupSize.x * a_Inputs.workGroupSize.y + a_Inputs.localInvocationID.y * a_Inputs.workGroupSize.x + a_Inputs.localInvocationID.x;
                a_Op(a_Inputs, shared);
            }
        }
    }
}
}

#endif // CPU_COMPUTE_HPP