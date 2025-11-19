#include <glm/vec3.hpp>

namespace Msg {
enum class ToneMappingType {
    None = -1, // skip the tonemapping
    ACES,
    Reinhard,
    Lottes,
    Neutral
};
// default values match ACES
struct LottesSettings {
    float hdrMax   = 8.0f;
    float contrast = 0.977f;
    float shoulder = 1.6f;
    float midIn    = 0.18;
    float midOut   = 0.267;
};
struct CameraBloomSettings {
    float intensity = 1.0f;
    float threshold = 1.0f;
    float smoothing = 10.f;
    float size      = 25.f;
    glm::vec3 tint  = { 1.f, 1.f, 1.f };
};
struct CameraAutoExposureSettings {
    bool enabled          = true;
    float minLuminance    = 0.0001f;
    float maxLuminance    = 1000.f;
    float key             = 0.5f;
    float adaptationSpeed = 1.0f;
};
struct CameraColorGradingSettings {
    CameraAutoExposureSettings autoExposure;
    float exposure   = 0.0f; // exposure offset, ignored if auto exposure is enabled
    float saturation = 0.0f; // saturation offset
    float contrast   = 0.0f; // contrast offset
    float hueShift   = 0.0f;
};
struct CameraToneMappingSettings {
    ToneMappingType toneMappingType = ToneMappingType::ACES;
    /** Lottes tone mapping curve settings */
    LottesSettings lottesSettings;
    /** Gamma used to convert from linear to Rec709 color space */
    float gamma = 2.2f;
};
struct CameraSettings {
    CameraColorGradingSettings colorGrading;
    CameraToneMappingSettings toneMapping;
    CameraBloomSettings bloom;
};
}