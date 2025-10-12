#include <MSG/Cmd/OGLCmdDraw.hpp>
#include <MSG/OGLCmdBufferExecutionState.hpp>
#include <MSG/OGLPipeline.hpp>
#include <MSG/OGLVertexArray.hpp>

#include <GL/glew.h>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char*)nullptr + (i))
#endif

static inline std::function<void(Msg::OGLCmdBufferExecutionState&)> GetFunctor(const Msg::OGLCmdDrawInfo& a_Info)
{
    if (a_Info.indexed) {
        return [info = a_Info](Msg::OGLCmdBufferExecutionState& a_State) {
            auto& gp = std::get<Msg::OGLGraphicsPipeline>(*a_State.pipeline);
            glDrawElementsInstancedBaseVertexBaseInstance(
                gp.inputAssemblyState.primitiveTopology,
                info.indexCount,
                gp.vertexInputState.vertexArray->indexDesc.type,
                BUFFER_OFFSET(info.indexOffset),
                info.instanceCount,
                info.vertexOffset,
                info.instanceOffset);
        };
    } else {
        return [info = a_Info](Msg::OGLCmdBufferExecutionState& a_State) {
            auto& gp = std::get<Msg::OGLGraphicsPipeline>(*a_State.pipeline);
            glDrawArraysInstancedBaseInstance(
                gp.inputAssemblyState.primitiveTopology,
                info.vertexOffset,
                info.vertexCount,
                info.instanceCount,
                info.instanceOffset);
        };
    }
}

Msg::OGLCmdDraw::OGLCmdDraw(const OGLCmdDrawInfo& a_Info)
    : _functor(GetFunctor(a_Info))
{
}