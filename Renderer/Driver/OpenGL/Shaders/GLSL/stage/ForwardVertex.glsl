#include <Random.glsl>

#ifdef __cplusplus
using namespace TabGraph::Renderer::GLSL;
#endif //__cplusplus

out gl_PerVertex
{
    vec4 gl_Position;
};
struct TransformUBO {
    // vec3 position;
    // vec3 scale;
    // vec4 rotation;
    mat4 matrix;
};
layout(binding = 0) uniform CameraBlock
{
    // TransformUBO transform;
    mat4 projection;
    mat4 view;
}
u_Camera;
layout(binding = 1) uniform TransformBlock
{
    TransformUBO u_ModelTransform;
};
layout(location = 0) in vec3 in_Position;
void main()
{
    vec4 worldPos = u_ModelTransform.matrix * vec4(in_Position, 1);
    gl_Position   = u_Camera.projection * u_Camera.view * worldPos;
}
