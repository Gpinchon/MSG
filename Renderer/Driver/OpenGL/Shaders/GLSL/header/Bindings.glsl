#ifndef BINDINGS_GLSL
#define BINDINGS_GLSL

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
#define UBO_CAMERA    0
#define UBO_TRANSFORM 1
#define UBO_MATERIAL  2

// Shader Storage Buffers
#define SSBO_VTFS_LIGHTS   0
#define SSBO_VTFS_CLUSTERS 1

// Samplers
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
#define SAMPLERS_MATERIAL_COUNT          (SAMPLERS_MATERIAL_METROUGH_MR + 1)
#define SAMPLERS_BRDF_LUT                7
#define SAMPLERS_GGX_LUT                 8

// Forward fragment shader output
#define OUTPUT_FRAG_MATERIAL 0
#define OUTPUT_FRAG_NORMAL   1
#define OUTPUT_FRAG_VELOCITY 2
#define OUTPUT_FRAG_FINAL    3
#define OUTPUT_FRAG_COUNT    4

#endif // BINDINGS_GLSL