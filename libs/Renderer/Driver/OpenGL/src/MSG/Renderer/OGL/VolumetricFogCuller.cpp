#pragma once

#include <MSG/OGLTexture3D.hpp>
#include <MSG/Renderer/OGL/VolumetricFogCuller.hpp>

#include <GL/glew.h>

#define VOLUMETRIC_FOG_WIDTH  256
#define VOLUMETRIC_FOG_HEIGHT 256
#define VOLUMETRIC_FOG_DEPTH  256

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

MSG::Renderer::VolumetricFogCuller::VolumetricFogCuller(OGLContext& a_Ctx)
    : image(GetImageInfo())
    , texture(std::make_shared<OGLTexture3D>(a_Ctx, GetTextureInfo()))
{
    image.Allocate();
}

void MSG::Renderer::VolumetricFogCuller::Update(const Scene& a_Scene)
{
}