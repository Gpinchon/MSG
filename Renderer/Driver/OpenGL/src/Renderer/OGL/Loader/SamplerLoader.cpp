#include <Renderer/OGL/Loader/SamplerLoader.hpp>
#include <Renderer/OGL/RAII/Sampler.hpp>

#include <SG/Core/Texture/Sampler.hpp>
#include <Tools/LazyConstructor.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer {
static inline auto ToGL(const SG::TextureSampler::CompareFunc& a_Func)
{
    switch (a_Func) {
    case SG::TextureSampler::CompareFunc::LessEqual:
        return GL_LEQUAL;
    case SG::TextureSampler::CompareFunc::GreaterEqual:
        return GL_GEQUAL;
    case SG::TextureSampler::CompareFunc::Less:
        return GL_LESS;
    case SG::TextureSampler::CompareFunc::Greater:
        return GL_GREATER;
    case SG::TextureSampler::CompareFunc::Equal:
        return GL_EQUAL;
    case SG::TextureSampler::CompareFunc::NotEqual:
        return GL_NOTEQUAL;
    case SG::TextureSampler::CompareFunc::Always:
        return GL_ALWAYS;
    case SG::TextureSampler::CompareFunc::Never:
        return GL_NEVER;
    }
    return GL_NONE;
}
static inline auto ToGL(const SG::TextureSampler::CompareMode& a_Mode)
{
    switch (a_Mode) {
    case SG::TextureSampler::CompareMode::None:
        return GL_NONE;
    case SG::TextureSampler::CompareMode::CompareRefToTexture:
        return GL_COMPARE_REF_TO_TEXTURE;
    }
    return GL_NONE;
}
static inline auto ToGL(const SG::TextureSampler::Filter& a_Filter)
{
    switch (a_Filter) {
    case SG::TextureSampler::Filter::Nearest:
        return GL_NEAREST;
    case SG::TextureSampler::Filter::Linear:
        return GL_LINEAR;
    case SG::TextureSampler::Filter::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
    case SG::TextureSampler::Filter::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
    case SG::TextureSampler::Filter::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
    case SG::TextureSampler::Filter::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    }
    return GL_NONE;
}
static inline auto ToGL(const SG::TextureSampler::Wrap& a_Wrap)
{
    switch (a_Wrap) {
    case SG::TextureSampler::Wrap::Repeat:
        return GL_REPEAT;
    case SG::TextureSampler::Wrap::ClampToBorder:
        return GL_CLAMP_TO_BORDER;
    case SG::TextureSampler::Wrap::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
    case SG::TextureSampler::Wrap::MirroredRepeat:
        return GL_MIRRORED_REPEAT;
    case SG::TextureSampler::Wrap::MirroredClampToEdge:
        return GL_MIRROR_CLAMP_TO_EDGE;
    }
    return GL_NONE;
}

static inline auto ToGL(SG::TextureSampler& a_Sampler)
{
    RAII::SamplerParameters parameters {};
    parameters.borderColor = a_Sampler.GetBorderColor();
    parameters.compareFunc = ToGL(a_Sampler.GetCompareFunc());
    parameters.compareMode = ToGL(a_Sampler.GetCompareMode());
    parameters.magFilter   = ToGL(a_Sampler.GetMagFilter());
    parameters.maxLOD      = a_Sampler.GetMaxLOD();
    parameters.minFilter   = ToGL(a_Sampler.GetMinFilter());
    parameters.minLOD      = a_Sampler.GetMinLOD();
    parameters.wrapR       = ToGL(a_Sampler.GetWrapR());
    parameters.wrapS       = ToGL(a_Sampler.GetWrapS());
    parameters.wrapT       = ToGL(a_Sampler.GetWrapT());
    return parameters;
}

std::shared_ptr<RAII::Sampler> SamplerLoader::operator()(RAII::Context& a_Context, SG::TextureSampler* a_Sampler)
{
    auto factory = Tools::LazyConstructor([&context = a_Context, sampler = a_Sampler] {
        return RAII::MakePtr<RAII::Sampler>(context, ToGL(*sampler));
    });
    return GetOrCreate(a_Sampler, factory);
}
}
