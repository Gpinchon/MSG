#include <MSG/Cmd/OGLCmdDraw.hpp>
#include <MSG/OGLCmdBufferState.hpp>
#include <MSG/OGLPipeline.hpp>
#include <MSG/OGLVertexArray.hpp>

#include <GL/glew.h>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char*)nullptr + (i))
#endif

static inline std::function<void(MSG::OGLCmdBufferState&)> GetFunctor(const MSG::OGLCmdDrawInfo& a_Info)
{
    if (a_Info.indexed) {
        return [info = a_Info](MSG::OGLCmdBufferState& a_State) {
            auto& gp = std::get<MSG::OGLGraphicsPipeline>(*a_State.pipeline);
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
        return [info = a_Info](MSG::OGLCmdBufferState& a_State) {
            auto& gp = std::get<MSG::OGLGraphicsPipeline>(*a_State.pipeline);
            glDrawArraysInstancedBaseInstance(
                gp.inputAssemblyState.primitiveTopology,
                info.vertexOffset,
                info.vertexCount,
                info.instanceCount,
                info.instanceOffset);
        };
    }
}

MSG::OGLCmdDraw::OGLCmdDraw(const OGLCmdDrawInfo& a_Info)
    : _functor(GetFunctor(a_Info))
{
}