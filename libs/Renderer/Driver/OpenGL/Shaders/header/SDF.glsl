#ifndef SDF_GLSL
#define SDF_GLSL

#include <Functions.glsl>
#include <Types.glsl>

#define SHAPE_TYPE_CUBE   0
#define SHAPE_TYPE_SPHERE 1

#define SHAPE_COMB_OP_ADD     0
#define SHAPE_COMB_OP_SUB     1
#define SHAPE_COMB_OP_INT     2
#define SHAPE_COMB_OP_XOR     3
#define SHAPE_COMB_MAX_SHAPES 32

#ifdef __cplusplus
namespace Msg::Renderer::GLSL {
#endif //__cplusplus
struct Cube {
    vec3 center;
    uint _padding1;
    vec3 halfSize;
    uint _padding2;
};
struct Sphere {
    vec3 center;
    float radius;
    uint _padding1[4];
};
struct ShapeCombShape {
    uint type; // the type of shape's SSBO to query
    uint index; // the shape's index to get inside SSBO
    uint op; // the operand to use for this shape (add, sub, intersect, xor)
    uint _padding;
};
struct ShapeComb {
    mat4x4 transform;
    uint count;
    uint _padding[3];
    ShapeCombShape shapes[SHAPE_COMB_MAX_SHAPES];
};

INLINE float Distance(IN(Cube) a_Cube, IN(vec3) a_Position, IN(mat4x4) a_Transform)
{
    vec4 p = a_Transform * vec4(a_Position - a_Cube.center, 1);
    vec3 d = abs(vec3(p) / p.w) - a_Cube.halfSize;
    return min(max(d.x, max(d.y, d.z)), 0.f) + length(max(d, 0.f));
}

INLINE float Distance(IN(Sphere) a_Sphere, IN(vec3) a_Position, IN(mat4x4) a_Transform)
{
    vec4 p = a_Transform * vec4(a_Position - a_Sphere.center, 1.f);
    return length(vec3(p) / p.w) - a_Sphere.radius;
}

INLINE float ShapeUnion(float d1, float d2) { return min(d1, d2); }
INLINE float ShapeSubtraction(float d1, float d2) { return max(-d1, d2); }
INLINE float ShapeIntersection(float d1, float d2) { return max(d1, d2); }
INLINE float ShapeXor(float d1, float d2) { return max(ShapeUnion(d1, d2), -ShapeIntersection(d1, d2)); }

#ifdef __cplusplus
static_assert(sizeof(Cube) % 16 == 0);
static_assert(sizeof(Cube) == sizeof(Cube));
static_assert(sizeof(ShapeComb) % 16 == 0);
}
#endif //__cplusplus
#endif // SDF_GLSL