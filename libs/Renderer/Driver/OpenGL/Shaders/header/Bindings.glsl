#ifndef BINDINGS_GLSL
#define BINDINGS_GLSL

// Cascaded fog specific
#define FOG_CASCADE_COUNT 3

// Vertex attributes
#define ATTRIB_TEXCOORD_COUNT 4
#define ATTRIB_POSITION       0
#define ATTRIB_NORMAL         1
#define ATTRIB_TANGENT        2
#define ATTRIB_TEXCOORD       3
#define ATTRIB_COLOR          (ATTRIB_TEXCOORD + ATTRIB_TEXCOORD_COUNT)
#define ATTRIB_JOINTS         (ATTRIB_COLOR + 1)
#define ATTRIB_WEIGHTS        (ATTRIB_JOINTS + 1)
#define ATTRIB_COUNT          (ATTRIB_WEIGHTS + 1)

// Uniform Buffers
#define UBO_FRAME_INFO   0
#define UBO_CAMERA       1
#define UBO_TRANSFORM    2
#define UBO_MATERIAL     3
#define UBO_FWD_IBL      4
#define UBO_FOG_SETTINGS 6
#define UBO_FOG_CAMERA   7

// Shader Storage Buffers
#define SSBO_VTFS_LIGHTS        0
#define SSBO_VTFS_CLUSTERS      1
#define SSBO_MESH_SKIN          2
#define SSBO_MESH_SKIN_PREV     3
#define SSBO_SHADOW_DATA        4
#define SSBO_SHADOW_VIEWPORTS   5
#define SSBO_SHADOW_DEPTH_RANGE 6

// Samplers
#define SAMPLERS_SKYBOX                  0
#define SAMPLERS_MATERIAL                0
#define SAMPLERS_MATERIAL_BASE_NORMAL    0
#define SAMPLERS_MATERIAL_BASE_OCCLUSION 1
#define SAMPLERS_MATERIAL_BASE_EMISSIVE  2
#define SAMPLERS_MATERIAL_SHEEN_COLOR    3
#define SAMPLERS_MATERIAL_SHEEN_ROUGH    4
#define SAMPLERS_MATERIAL_SPECGLOSS_DIFF 5
#define SAMPLERS_MATERIAL_SPECGLOSS_SG   6
#define SAMPLERS_MATERIAL_METROUGH_COL   5
#define SAMPLERS_MATERIAL_METROUGH_MR    6
#define SAMPLERS_MATERIAL_COUNT          7
#define SAMPLERS_BRDF_LUT                (SAMPLERS_MATERIAL + SAMPLERS_MATERIAL_COUNT)
#define SAMPLERS_IBL                     (SAMPLERS_BRDF_LUT + 1)
#define SAMPLERS_IBL_COUNT               3
#define SAMPLERS_SHADOW                  (SAMPLERS_IBL + SAMPLERS_IBL_COUNT)
#define SAMPLERS_SHADOW_COUNT            5
#define SAMPLERS_FOG                     (SAMPLERS_SHADOW + SAMPLERS_SHADOW_COUNT)
#define SAMPLERS_FOG_COUNT               FOG_CASCADE_COUNT

// Forward opaque fragment shader output
#define OUTPUT_FRAG_FWD_OPAQUE_COLOR    0
#define OUTPUT_FRAG_FWD_OPAQUE_VELOCITY 1
#define OUTPUT_FRAG_FWD_OPAQUE_COUNT    2

// OIT images
#define IMG_OIT_COLORS   0
#define IMG_OIT_VELOCITY 1
#define IMG_OIT_DEPTH    2

// Forward compositing fragment shader output
#define OUTPUT_FRAG_OIT_COLOR    0
#define OUTPUT_FRAG_OIT_VELOCITY 1
#define OUTPUT_FRAG_OIT_COUNT    2

// Deferred
#define OUTPUT_FRAG_DFD_GBUFFER0 0
#define OUTPUT_FRAG_DFD_GBUFFER1 1
#define OUTPUT_FRAG_DFD_VELOCITY 2
#define OUTPUT_FRAG_DFD_FINAL    3
#define OUTPUT_FRAG_DFD_COUNT    4

#endif // BINDINGS_GLSL