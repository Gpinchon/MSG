/*
* @Author: gpinchon
* @Date:   2020-08-03 16:05:52
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-06 19:57:04
*/

#define SIMD_EPSILON 0.0001

#include "Physics/CollisionAlgorithmGJKRaycast.hpp"
#include "Physics/Collision.hpp"
#include "Physics/RigidBody.hpp"
#include "Physics/SimplexSolver.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

namespace TabGraph::Physics {
glm::mat4 CreateTransformMatrix(const glm::vec3& position, const glm::quat& rotation)
{
    return glm::translate(position) * glm::mat4(rotation);
}

bool CollisionAlgorithmGJKRaycast::Collides(const std::shared_ptr<RigidBody>& a, const std::shared_ptr<RigidBody>& b, Collision& out)
{
    auto colliderA(a->GetBoundingElement());
    auto colliderB(b->GetBoundingElement());

    SimplexSolver simplex;

    glm::vec3 linVelA, linVelB;
    linVelA = a->GetLastPosition() - a->GetLocalPosition();
    linVelB = b->GetLastPosition() - b->GetLocalPosition();

    float lambda = float(0.);

    auto interpolatedPositionA{ a->GetLocalPosition() };
    auto interpolatedRotationA{ a->GetLocalRotation() };
    auto interpolatedPositionB{ b->GetLocalPosition() };
    auto interpolatedRotationB{ b->GetLocalRotation() };

    ///take relative motion
    glm::vec3 r = linVelA - linVelB;
    glm::vec3 v;

    glm::vec3 supVertexA = colliderA->Project(-r, a->GetCurrentTransform()).Max();
    glm::vec3 supVertexB = colliderB->Project(r, b->GetCurrentTransform()).Max();
    v = supVertexA - supVertexB;
    int maxIter = 32;

    glm::vec3 n = glm::vec3(0.f);
    bool hasResult = false;
    //glm::vec3 c;

    //float lastLambda = lambda;

    float dist2 = length2(v);
    float epsilon = float(0.0001);
    glm::vec3 w /*, p*/;
    float VdotR;

    while ((dist2 > epsilon) && maxIter--) {
        supVertexA = colliderA->Project(-v, CreateTransformMatrix(interpolatedPositionA, interpolatedRotationA)).Max();
        supVertexB = colliderB->Project(v, CreateTransformMatrix(interpolatedPositionB, interpolatedRotationB)).Max();
        w = supVertexA - supVertexB;

        float VdotW = dot(v, w);

        if (lambda > 1.f) {
            return false;
        }

        if (VdotW > 0.f) {
            VdotR = dot(v, r);

            if (VdotR >= -(SIMD_EPSILON * SIMD_EPSILON))
                return false;
            else {
                lambda = lambda - VdotW / VdotR;
                //interpolate to next lambda
                //  x = s + lambda * r;

                interpolatedPositionA = glm::mix(a->GetLastPosition(), a->GetLocalPosition(), lambda);
                interpolatedRotationA = glm::mix(a->GetLastRotation(), a->GetLocalRotation(), lambda);
                interpolatedPositionB = glm::mix(b->GetLastPosition(), b->GetLocalPosition(), lambda);
                interpolatedRotationB = glm::mix(b->GetLastRotation(), b->GetLocalRotation(), lambda);
                //simplex.reset();
                //check next line
                w = supVertexA - supVertexB;
                //lastLambda = lambda;
                n = v;
                hasResult = true;
            }
        }
        ///Just like regular GJK only add the vertex if it isn't already (close) to current vertex, it would lead to divisions by zero and NaN etc.
        if (!simplex.InSimplex(w))
            simplex.AddVertex(w, supVertexA, supVertexB);

        if (simplex.Closest(v)) {
            dist2 = length2(v);
            //hasResult = true;
            //todo: check this normal for validity
            //n=v;
            //printf("V=%f , %f, %f\n",v[0],v[1],v[2]);
            //printf("DIST2=%f\n",dist2);
            //printf("numverts = %i\n",simplex.numVertices());
        } else {
            dist2 = 0.f;
        }
    }

    //int numiter = MAX_ITERATIONS - maxIter;
    //  printf("number of iterations: %d", numiter);

    if (!hasResult) {
        n = v;
    }
    //don't report a time of impact when moving 'away' from the hitnormal

    if (length2(n) > (SIMD_EPSILON * SIMD_EPSILON))
        _intersectionResult.SetNormal(normalize(n));
    else
        _intersectionResult.SetNormal(glm::vec3(0.f));

    //don't report time of impact for motion away from the contact normal (or causes minor penetration)
    if (dot(_intersectionResult.GetNormal(), r) >= -GetAllowedPenetration() || lambda > 1) {
        return false;
    }

    glm::vec3 hitA, hitB;
    simplex.ComputePoints(hitA, hitB);
    auto ab = hitA - hitB;
    auto d = -dot(ab, _intersectionResult.GetNormal());
    _intersectionResult.SetFraction(lambda);
    _intersectionResult.SetHitPointA(hitA);
    _intersectionResult.SetHitPointB(hitB);
    _intersectionResult.SetPenetrationDepth(d);
    out = Collision(Collision::CollideesPair(a, b), _intersectionResult.HitPoint(), _intersectionResult.GetNormal(), _intersectionResult.GetPenetrationDepth());
    return true;
}

float CollisionAlgorithmGJKRaycast::GetFraction() const
{
    return _intersectionResult.GetFraction();
}
}