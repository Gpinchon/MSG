/*
* @Author: gpinchon
* @Date:   2021-06-19 00:47:45
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 00:55:44
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <memory>

#include <glm/vec2.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Shapes {
class Geometry;
}
namespace Nodes {
class Scene;
}
namespace Textures {
class Texture2D;
}
namespace Renderer {
class Framebuffer;
}
namespace Core {
class Window;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class Declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
/**
 * @brief see https://www.khronos.org/opengl/wiki/Swap_Interval
*/
enum class SwapInterval {
    Adaptative = -1, //Adaptative V-Sync
    Immediate, //No V-Sync
    Synchronized //Enable V-Sync
};
struct ViewPort {
    glm::ivec2 offset{ 0 };
    glm::ivec2 size{ 0 };
};
class FrameRenderer : public std::enable_shared_from_this<FrameRenderer> {
public:
    FrameRenderer(std::weak_ptr<Core::Window> window);
    ~FrameRenderer();
    void SetViewPort(const glm::ivec2& min, const glm::ivec2& max);
    void SetViewPort(const glm::ivec2& size);
    uint32_t GetFrameNumber() const;
    const std::shared_ptr<Core::Window> GetWindow() const;
    const std::shared_ptr<Shapes::Geometry> GetDisplayQuad() const;
    const std::shared_ptr<Textures::Texture2D> GetDefaultBRDFLUT() const;
    void RenderFrame(std::shared_ptr<Nodes::Scene> scene);
    /**
        * @brief Sets the swap interval of this FrameRenderer
        * @param swapInterval : see https://www.khronos.org/opengl/wiki/Swap_Interval
    */
    void SetSwapInterval(SwapInterval swapInterval);
    /**
        * @return the current swap interval of this FrameBuffer
    */
    SwapInterval GetSwapInterval() const;
    //----------------Deferred Path-----------------------------
    /**
        * Deferred Render Target :
        * RT0 : BRDF CDiff (RGB), Ambient Occlusion (A) //Uint8_NormalizedRGBA
        * RT1 : BRDF F0 (RGB), BRDF Alpha (A)           //Uint8_NormalizedRGBA
        * RT2 : Normal (RGB)                            //Int16_NormalizedRGB
        * RT3 : Velocity (RG)                           //Float16_RG
        * RT4 : Color (Unlit/Emissive/Final Color)      //Float16_RGB
    */
    std::shared_ptr<Framebuffer> DeferredGeometryBuffer();
    /**
        * Deferred Lighting Render Target
        * RT0 : Diffuse    //Float16_RGB
        * RT1 : Reflection //Float16_RGB
    */
    std::shared_ptr<Framebuffer> DeferredLightingBuffer();
    //----------------------------------------------------------

    //----------------Forward Path------------------------------
    /**
        * Forward Transparent Render Target :
        * RT0 : Weighted Color (RGB), Opacity (A)        //Float16_RGBA
        * RT1 : Alpha Coverage                           //Uint8_NormalizedR
        * RT2 : Distortion                               //Uint8_NormalizedRGB
        * RT3 : Transmission Color (RGB), BRDF Alpha (A) //Uint8_NormalizedRGBA
    */
    std::shared_ptr<Framebuffer> ForwardTransparentRenderBuffer();
    //----------------------------------------------------------

    /**
        * Opaque Render Target :
        * RT0 : Color (Unlit/Emissive/Final Color) //Float16_RGB
    */
    std::shared_ptr<Framebuffer> OpaqueRenderBuffer();
    /**
        * RT0 : Final Render Buffer //Uint8_NormalizedRGB
    */
    std::shared_ptr<Framebuffer> FinalRenderBuffer();

    /**
        * RT0 : Previous Frame Render Buffer //Uint8_NormalizedRGB
    */
    std::shared_ptr<Framebuffer> PreviousRenderBuffer();

private:
    class Impl;
    friend Impl;
    std::unique_ptr<Impl> _impl;
};
}
