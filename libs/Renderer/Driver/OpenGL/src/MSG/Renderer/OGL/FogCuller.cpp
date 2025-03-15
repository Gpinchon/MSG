#pragma once

#include <MSG/FogArea.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/OGLTexture3D.hpp>
#include <MSG/Renderer/OGL/FogCuller.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Scene.hpp>

#include <GL/glew.h>

#define VOLUMETRIC_FOG_WIDTH  256
#define VOLUMETRIC_FOG_HEIGHT 256
#define VOLUMETRIC_FOG_DEPTH  256
#define VOLUMETRIC_FOG_COUNT  (VOLUMETRIC_FOG_WIDTH * VOLUMETRIC_FOG_HEIGHT * VOLUMETRIC_FOG_DEPTH)

MSG::OGLTexture3DInfo GetTextureInfo()
{
    return {
        .width       = VOLUMETRIC_FOG_WIDTH,
        .height      = VOLUMETRIC_FOG_HEIGHT,
        .depth       = VOLUMETRIC_FOG_DEPTH,
        .sizedFormat = GL_RGBA8
    };
}

MSG::ImageInfo GetImageInfo()
{
    return {
        .width     = VOLUMETRIC_FOG_WIDTH,
        .height    = VOLUMETRIC_FOG_HEIGHT,
        .depth     = VOLUMETRIC_FOG_DEPTH,
        .pixelDesc = MSG::PixelSizedFormat::Uint8_NormalizedRGBA
    };
}

MSG::Renderer::FogCuller::FogCuller(Renderer::Impl& a_Renderer)
    : context(a_Renderer.context)
    , image(GetImageInfo())
    , texture(std::make_shared<OGLTexture3D>(context, GetTextureInfo()))
    , cullingProgram(a_Renderer.shaderCompiler.CompileProgram("FogCulling"))
{
    image.Allocate();
}

void MSG::Renderer::FogCuller::Update(const Scene& a_Scene)
{
    auto& registry        = *a_Scene.GetRegistry();
    glm::vec4 globalColor = { 1, 1, 1, 0 }; // figure out global color
    for (auto& entity : a_Scene.GetVisibleEntities().fogAreas) {
        auto& fogArea = registry.GetComponent<FogArea>(entity);
        if (glm::any(glm::isinf(fogArea.GetHalfSize()))) {
            assert(fogArea.GetGrid().GetSize() == glm::uvec3(1) && "Infinite fog sources can only have one color.");
            auto color = fogArea.GetGrid().Load({ 0, 0, 0 });
            globalColor *= glm::vec4(glm::vec3(color), 1);
            globalColor.a += color.a;
        }
    }
    if (globalColor.a == 0)
        return;
    image.Fill(globalColor); // clear image
    texture->UploadLevel(0, image);
    ExecuteOGLCommand(context, [texture = texture, cullingProgram = cullingProgram] {
        glBindImageTexture(0, *texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
        glUseProgram(*cullingProgram);
        // dispatch compute
        glDispatchCompute(VOLUMETRIC_FOG_WIDTH, VOLUMETRIC_FOG_HEIGHT, VOLUMETRIC_FOG_DEPTH);
        glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
        glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
        glUseProgram(0);
    });
}