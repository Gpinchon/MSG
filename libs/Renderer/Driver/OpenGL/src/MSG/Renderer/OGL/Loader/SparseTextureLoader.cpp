#include <MSG/ImageUtils.hpp>
#include <MSG/Renderer/OGL/Loader/SparseTextureLoader.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/SparseTexture.hpp>
#include <MSG/Renderer/OGL/ToGL.hpp>
#include <MSG/TextureUtils.hpp>

#include <MSG/Debug.hpp>
#include <MSG/Tools/LazyConstructor.hpp>

glm::uvec3 GetSparseFormatPageSize(MSG::OGLContext& a_Ctx, const uint32_t& a_Target, const uint32_t& a_SizedFormat)
{
    static std::map<std::pair<uint32_t, uint32_t>, glm::ivec3> s_PageSize;
    auto key = std::make_pair(a_Target, a_SizedFormat);
    auto itr = s_PageSize.find(key);
    if (itr == s_PageSize.end()) {
        itr               = s_PageSize.emplace(key, glm::ivec3(0)).first;
        int32_t pageSizes = 0;
        MSG::ExecuteOGLCommand(a_Ctx, [&pageSizes, &pageSize = itr->second, &a_Target, &a_SizedFormat]() mutable {
            glGetInternalformativ(a_Target, a_SizedFormat, GL_NUM_VIRTUAL_PAGE_SIZES_ARB, 1, &pageSizes);
            glGetInternalformativ(a_Target, a_SizedFormat, GL_VIRTUAL_PAGE_SIZE_X_ARB, 1, &pageSize.x);
            glGetInternalformativ(a_Target, a_SizedFormat, GL_VIRTUAL_PAGE_SIZE_Y_ARB, 1, &pageSize.y);
            glGetInternalformativ(a_Target, a_SizedFormat, GL_VIRTUAL_PAGE_SIZE_Z_ARB, 1, &pageSize.z); }, true);
        if (pageSizes == 0)
            errorWarning("Format does not support sparse allocation !");
        debugStream << "PageSize : " << itr->second.x << ' ' << itr->second.y << ' ' << itr->second.z << std::endl;
    }
    return itr->second;
}

uint32_t GetSparseMaxTextureSize(MSG::OGLContext& a_Ctx)
{
    static int32_t s_MaxSize = 0;
    if (s_MaxSize == 0)
        MSG::ExecuteOGLCommand(a_Ctx, [&maxSize = s_MaxSize]() mutable { glGetIntegerv(GL_MAX_SPARSE_TEXTURE_SIZE_ARB, &maxSize); }, true);
    return s_MaxSize;
}

template <typename T>
static inline T RoundUp(const T& numToRound, const T& multiple)
{
    return static_cast<T>((1 + (numToRound - 1) / multiple) * multiple);
}

static inline glm::uvec3 RoundUp(const glm::uvec3& a_Val, const glm::uvec3& a_Multiple)
{
    return {
        RoundUp(a_Val[0], a_Multiple[0]),
        RoundUp(a_Val[1], a_Multiple[1]),
        RoundUp(a_Val[2], a_Multiple[2]),
    };
}

std::shared_ptr<MSG::Renderer::SparseTexture> MSG::Renderer::SparseTextureLoader::operator()(Renderer::Impl& a_Rdr, const std::shared_ptr<Texture>& a_Txt)
{
    auto factory = Tools::LazyConstructor([&a_Rdr, &a_Txt] {
        auto compressed = a_Txt->GetPixelDescriptor().GetSizedFormat() == MSG::PixelSizedFormat::DXT5_RGBA;
        auto texSize    = a_Txt->GetSize();
        auto maxSize    = GetSparseMaxTextureSize(a_Rdr.context);
        auto pageSize   = GetSparseFormatPageSize(a_Rdr.context, ToGL(a_Txt->GetType()), ToGL(a_Txt->GetPixelDescriptor().GetSizedFormat()));
        auto sideSize   = glm::max(texSize[0], texSize[1]); // despite what the ARB_sparse_texture specs says, it REQUIRES square textures
        // don't make texture sparse if smaller than pageSize to save space
        if (glm::all(glm::lessThanEqual(glm::uvec3(sideSize, sideSize, texSize[2]), pageSize)))
            return std::make_shared<SparseTexture>(a_Rdr.context, a_Txt, false);
        auto requiredSize = glm::min(
            glm::uvec3(
                RoundUp(sideSize, pageSize[0]),
                RoundUp(sideSize, pageSize[1]),
                texSize[2] > pageSize[2] ? RoundUp(texSize[2], pageSize[2]) : texSize[2]),
            maxSize);
        if (requiredSize != texSize) {
            errorWarning("Texture size is not square and/or a multiple of pageSize, resizing...");
            auto baseImage = a_Txt->front();
            if (compressed)
                *baseImage = ImageDecompress(*baseImage);
            *baseImage = ImageResize(*baseImage, requiredSize);
            *a_Txt     = Texture(a_Txt->GetType(), baseImage);
            TextureGenerateMipmaps(*a_Txt);
            if (compressed)
                TextureCompress(*a_Txt);
        }
        return std::make_shared<SparseTexture>(a_Rdr.context, a_Txt, true);
    });
    return cache.GetOrCreate(a_Txt.get(), factory);
}