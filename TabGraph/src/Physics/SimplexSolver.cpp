/*
* @Author: gpinchon
* @Date:   2020-07-21 18:21:59
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-07-22 22:24:49
*/

#include "Physics/SimplexSolver.hpp"
#include <iostream>

bool SimplexSolver::_DoEdge(SimplexClosestSubSet& result, const glm::vec3& p)
{
    const auto& a = VertexW(0);
    const auto& b = VertexW(1);
    return _DoEdge(result, p, a, b);
}

bool SimplexSolver::_DoEdge(SimplexClosestSubSet& result, const glm::vec3& o, const glm::vec3& a, const glm::vec3& b)
{
    auto ao = o - a;
    auto ab = b - a;
    auto t = dot(ab, ao);

    if (t > 0) {
        auto dotVV = dot(ab, ab);
        if (t < dotVV) {
            t /= dotVV;
            ao -= t * ab;
            result.usedVertices.a = true;
            result.usedVertices.b = true;
        } else {
            t = 1;
            ao -= ab;
            result.usedVertices.b = true;
        }
    } else {
        t = 0;
        result.usedVertices.a = true;
    }
    result.SetBarycentricCoords(1 - t, t);
    return true;
    /*auto A(s.at(1));
    auto B(s.at(0));
    auto O(origin);
    auto AB(B - A);
    auto AO(o - A);
    if (dot(AB, AO) < 0) {
        // Point is closest feature
        std::cout << __FUNCTION__ << " : A\n";
        axis = AO;
        s = { A };
    } else {
        // Line is closest feature
        std::cout << __FUNCTION__ << " : AB\n";
        axis = cross(AB, cross(AO, AB));
    }
    return false;*/
}

bool SimplexSolver::_DoTriangle(SimplexClosestSubSet& result, const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
    result.usedVertices.Reset();
    // Check if P in vertex region outside A
    auto ab = b - a;
    auto ac = c - a;
    auto ap = p - a;
    float d1 = dot(ab, ap);
    float d2 = dot(ac, ap);
    if (d1 <= float(0.0) && d2 <= float(0.0)) {
        result.closestPoint = a;
        result.usedVertices.a = true;
        result.SetBarycentricCoords(1, 0, 0);
        return true; // a; // barycentric coordinates (1,0,0)
    }
    // Check if P in vertex region outside B
    auto bp = p - b;
    float d3 = dot(ab, bp);
    float d4 = dot(ac, bp);
    if (d3 >= float(0.0) && d4 <= d3) {
        result.closestPoint = b;
        result.usedVertices.b = true;
        result.SetBarycentricCoords(0, 1, 0);

        return true; // b; // barycentric coordinates (0,1,0)
    }
    // Check if P in edge region of AB, if so return projection of P onto AB
    float vc = d1 * d4 - d3 * d2;
    if (vc <= float(0.0) && d1 >= float(0.0) && d3 <= float(0.0)) {
        float v = d1 / (d1 - d3);
        result.closestPoint = a + v * ab;
        result.usedVertices.a = true;
        result.usedVertices.b = true;
        result.SetBarycentricCoords(1 - v, v, 0);
        return true;
        //return a + v * ab; // barycentric coordinates (1-v,v,0)
    }
    // Check if P in vertex region outside C
    auto cp = p - c;
    float d5 = dot(ab, cp);
    float d6 = dot(ac, cp);
    if (d6 >= float(0.0) && d5 <= d6) {
        result.closestPoint = c;
        result.usedVertices.c = true;
        result.SetBarycentricCoords(0, 0, 1);
        return true; //c; // barycentric coordinates (0,0,1)
    }
    // Check if P in edge region of AC, if so return projection of P onto AC
    float vb = d5 * d2 - d1 * d6;
    if (vb <= float(0.0) && d2 >= float(0.0) && d6 <= float(0.0)) {
        float w = d2 / (d2 - d6);
        result.closestPoint = a + w * ac;
        result.usedVertices.a = true;
        result.usedVertices.c = true;
        result.SetBarycentricCoords(1 - w, 0, w);
        return true;
        //return a + w * ac; // barycentric coordinates (1-w,0,w)
    }
    // Check if P in edge region of BC, if so return projection of P onto BC
    float va = d3 * d6 - d5 * d4;
    if (va <= float(0.0) && (d4 - d3) >= float(0.0) && (d5 - d6) >= float(0.0)) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        result.closestPoint = b + w * (c - b);
        result.usedVertices.b = true;
        result.usedVertices.c = true;
        result.SetBarycentricCoords(0, 1 - w, w);
        return true;
        // return b + w * (c - b); // barycentric coordinates (0,1-w,w)
    }
    // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
    float denom = float(1.0) / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    result.closestPoint = a + ab * v + ac * w;
    result.usedVertices.a = true;
    result.usedVertices.b = true;
    result.usedVertices.c = true;
    result.SetBarycentricCoords(1 - v - w, v, w);

    return true;
}

bool SimplexSolver::_DoTriangle(SimplexClosestSubSet& result, const glm::vec3& p)
{
    auto a = VertexW(0);
    auto b = VertexW(1);
    auto c = VertexW(2);
    return _DoTriangle(result, p, a, b, c);
}

auto PointOutsideOfPlane(const glm::vec3& p, const glm::vec3& a,
    const glm::vec3& b, const glm::vec3& c,
    const glm::vec3& d)
{
    auto normal = cross(b - a, c - a);

    auto signp = dot(p - a, normal); // [AP AB AC]
    auto signd = dot(d - a, normal); // [AD AB AC]

    if (signd * signd < (1e-4 * 1e-4)) {
        //      printf("affine dependent/degenerate\n");//
        return -1;
    }
    // Points on opposite sides if expression signs are opposite
    return int(signp * signd < 0);
}

bool SimplexSolver::_DoTetrahedron(SimplexClosestSubSet& result, const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d)
{
    SimplexClosestSubSet tempResult;
    result.closestPoint = p;
    result.usedVertices.Reset();
    result.usedVertices.a = true;
    result.usedVertices.b = true;
    result.usedVertices.c = true;
    result.usedVertices.d = true;

    auto pointOutsideABC = PointOutsideOfPlane(p, a, b, c, d);
    auto pointOutsideACD = PointOutsideOfPlane(p, a, c, d, b);
    auto pointOutsideADB = PointOutsideOfPlane(p, a, d, b, c);
    auto pointOutsideBDC = PointOutsideOfPlane(p, b, d, c, a);

    if (pointOutsideABC < 0 || pointOutsideACD < 0 || pointOutsideADB < 0 || pointOutsideBDC < 0) {
        result.degenerate = true;
        return false;
    }

    if (!pointOutsideABC && !pointOutsideACD && !pointOutsideADB && !pointOutsideBDC) {
        return false;
    }

    auto bestSqDist = std::numeric_limits<float>::max();
    // If point outside face abc then compute closest point on abc
    if (pointOutsideABC) {
        _DoTriangle(tempResult, p, a, b, c);
        auto q = tempResult.closestPoint;

        auto sqDist = dot(q - p, q - p);
        // Update best closest point if (squared) distance is less than current best
        if (sqDist < bestSqDist) {
            bestSqDist = sqDist;
            result.closestPoint = q;
            result.usedVertices.Reset();
            result.usedVertices.a = tempResult.usedVertices.a;
            result.usedVertices.b = tempResult.usedVertices.b;
            result.usedVertices.c = tempResult.usedVertices.c;
            result.SetBarycentricCoords(
                tempResult.BarycentricCoords()[VERTA],
                tempResult.BarycentricCoords()[VERTB],
                tempResult.BarycentricCoords()[VERTC],
                0);
        }
    }

    // Repeat test for face acd
    if (pointOutsideACD) {
        _DoTriangle(tempResult, p, a, c, d);
        auto q = tempResult.closestPoint;
        //convert result bitmask!

        auto sqDist = dot(q - p, q - p);
        if (sqDist < bestSqDist) {
            bestSqDist = sqDist;
            result.closestPoint = q;
            result.usedVertices.Reset();
            result.usedVertices.a = tempResult.usedVertices.a;
            result.usedVertices.c = tempResult.usedVertices.b;
            result.usedVertices.d = tempResult.usedVertices.c;
            result.SetBarycentricCoords(
                tempResult.BarycentricCoords()[VERTA],
                0,
                tempResult.BarycentricCoords()[VERTB],
                tempResult.BarycentricCoords()[VERTC]);
        }
    }
    // Repeat test for face adb

    if (pointOutsideADB) {
        _DoTriangle(tempResult, p, a, d, b);
        auto q = tempResult.closestPoint;
        //convert result bitmask!

        auto sqDist = dot(q - p, q - p);
        if (sqDist < bestSqDist) {
            bestSqDist = sqDist;
            result.closestPoint = q;
            result.usedVertices.Reset();
            result.usedVertices.a = tempResult.usedVertices.a;
            result.usedVertices.b = tempResult.usedVertices.c;
            result.usedVertices.d = tempResult.usedVertices.b;
            result.SetBarycentricCoords(
                tempResult.BarycentricCoords()[VERTA],
                tempResult.BarycentricCoords()[VERTC],
                0,
                tempResult.BarycentricCoords()[VERTB]);
        }
    }
    // Repeat test for face bdc

    if (pointOutsideBDC) {
        _DoTriangle(tempResult, p, b, d, c);
        auto q = tempResult.closestPoint;
        //convert result bitmask!
        auto sqDist = dot(q - p, q - p);
        if (sqDist < bestSqDist) {
            bestSqDist = sqDist;
            result.closestPoint = q;
            result.usedVertices.Reset();
            result.usedVertices.b = tempResult.usedVertices.a;
            result.usedVertices.c = tempResult.usedVertices.c;
            result.usedVertices.d = tempResult.usedVertices.b;

            result.SetBarycentricCoords(
                0,
                tempResult.BarycentricCoords()[VERTA],
                tempResult.BarycentricCoords()[VERTC],
                tempResult.BarycentricCoords()[VERTB]);
        }
    }

    //help! we ended up full !

    if (result.usedVertices.a && result.usedVertices.b && result.usedVertices.c && result.usedVertices.d) {
        return true;
    }

    return true;
}

bool SimplexSolver::_DoTetrahedron(SimplexClosestSubSet& result, const glm::vec3& p)
{
    const auto a = VertexW(0);
    const auto b = VertexW(1);
    const auto c = VertexW(2);
    const auto d = VertexW(3);
    return _DoTetrahedron(result, p, a, b, c, d);
}

bool SimplexSolver::_DoPoint(SimplexClosestSubSet& result)
{
    const auto a = VertexW(0);
    return _DoPoint(result, a);
}

bool SimplexSolver::_DoPoint(SimplexClosestSubSet& result, const glm::vec3& /*a*/)
{
    result.Reset();
    result.usedVertices.a = true;
    result.SetBarycentricCoords(1, 0, 0, 0);
    return true;
}

bool SimplexSolver::UpdateClosest(const glm::vec3& origin = glm::vec3(0.f))
{
    if (_needsUpdate) {
        _cachedResult.Reset();

        _needsUpdate = false;

        switch (Size()) {
        case 0:
            _cachedValidClosest = false;
            break;
        case 1: {
            _cachedP1 = _pointsP[0];
            _cachedP2 = _pointsQ[0];
            _cachedV = _cachedP1 - _cachedP2; //== _verticesW[0]
            _cachedResult.Reset();
            _cachedResult.SetBarycentricCoords(float(1.), float(0.), float(0.), float(0.));
            _cachedValidClosest = _cachedResult.IsValid();
            break;
        };
        case 2: {
            //closest point origin from line segment
            const glm::vec3& from = _verticesW[0];
            const glm::vec3& to = _verticesW[1];
            glm::vec3 nearest;

            glm::vec3 p = origin;
            glm::vec3 diff = p - from;
            glm::vec3 v = to - from;
            float t = dot(v, diff);

            if (t > 0) {
                float dotVV = dot(v, v);
                if (t < dotVV) {
                    t /= dotVV;
                    diff -= t * v;
                    _cachedResult.usedVertices.a = true;
                    _cachedResult.usedVertices.b = true;
                } else {
                    t = 1;
                    diff -= v;
                    //reduce to 1 point
                    _cachedResult.usedVertices.b = true;
                }
            } else {
                t = 0;
                //reduce to 1 point
                _cachedResult.usedVertices.a = true;
            }
            _cachedResult.SetBarycentricCoords(1 - t, t);
            nearest = from + t * v;

            _cachedP1 = _pointsP[0] + t * (_pointsP[1] - _pointsP[0]);
            _cachedP2 = _pointsQ[0] + t * (_pointsQ[1] - _pointsQ[0]);
            _cachedV = _cachedP1 - _cachedP2;

            ReduceVertices(_cachedResult.usedVertices);

            _cachedValidClosest = _cachedResult.IsValid();
            break;
        }
        case 3: {
            //closest point origin from triangle
            glm::vec3 p = origin;

            const glm::vec3& a = _verticesW[0];
            const glm::vec3& b = _verticesW[1];
            const glm::vec3& c = _verticesW[2];

            _DoTriangle(_cachedResult, p, a, b, c);

            _cachedP1 = _pointsP[0] * _cachedResult.BarycentricCoords()[0] + _pointsP[1] * _cachedResult.BarycentricCoords()[1] + _pointsP[2] * _cachedResult.BarycentricCoords()[2];

            _cachedP2 = _pointsQ[0] * _cachedResult.BarycentricCoords()[0] + _pointsQ[1] * _cachedResult.BarycentricCoords()[1] + _pointsQ[2] * _cachedResult.BarycentricCoords()[2];

            _cachedV = _cachedP1 - _cachedP2;

            ReduceVertices(_cachedResult.usedVertices);
            _cachedValidClosest = _cachedResult.IsValid();

            break;
        }
        case 4: {
            glm::vec3 p = origin;

            const glm::vec3& a = _verticesW[0];
            const glm::vec3& b = _verticesW[1];
            const glm::vec3& c = _verticesW[2];
            const glm::vec3& d = _verticesW[3];

            bool hasSeparation = _DoTetrahedron(_cachedResult, p, a, b, c, d);
            //closestPtPointTetrahedron(p, a, b, c, d, _cachedResult);

            if (hasSeparation) {
                _cachedP1 = _pointsP[0] * _cachedResult.BarycentricCoords()[0] + _pointsP[1] * _cachedResult.BarycentricCoords()[1] + _pointsP[2] * _cachedResult.BarycentricCoords()[2] + _pointsP[3] * _cachedResult.BarycentricCoords()[3];

                _cachedP2 = _pointsQ[0] * _cachedResult.BarycentricCoords()[0] + _pointsQ[1] * _cachedResult.BarycentricCoords()[1] + _pointsQ[2] * _cachedResult.BarycentricCoords()[2] + _pointsQ[3] * _cachedResult.BarycentricCoords()[3];

                _cachedV = _cachedP1 - _cachedP2;
                ReduceVertices(_cachedResult.usedVertices);
            } else {
                //					printf("sub distance got penetration\n");

                if (_cachedResult.degenerate) {
                    _cachedValidClosest = false;
                } else {
                    _cachedValidClosest = true;
                    //degenerate case == false, penetration = true + zero
                    _cachedV = glm::vec3(float(0.), float(0.), float(0.));
                }
                break;
            }

            _cachedValidClosest = _cachedResult.IsValid();

            //closest point origin from tetrahedron
            break;
        }
        default: {
            _cachedValidClosest = false;
        }
        };
    }

    return _cachedValidClosest;
    /*
    if (!_needsUpdate)
        return _cachedValidClosest;
    //std::cout << "Size before operation " << int(Size()) << "\n";
    _cachedResult.Reset();
    _needsUpdate = false;
    switch (Size()) {
    case 0:
        _cachedValidClosest = false;
        break;
    case 1:
        _DoPoint(_cachedResult);
        break;
    case 2:
        _DoEdge(_cachedResult, origin);
        break;
    case 3:
        _DoTriangle(_cachedResult, origin);
        break;
    case 4:
        auto hasSeparation = _DoTetrahedron(_cachedResult, origin);
        if (!hasSeparation) {
            std::cout << "Got Penetration\n";
            if (_cachedResult.degenerate) {
                std::cout << "DEGENERATE\n";
            } else {
                _cachedV = glm::vec3(0.f);
            }
            break;
        }
        break;
    }
    _cachedP1 = glm::vec3(0);
    _cachedP2 = glm::vec3(0);
    for (auto i = 0; i < Size(); ++i) {
        _cachedP1 += PointP(i) * _cachedResult.BarycentricCoords()[i];
        _cachedP2 += PointQ(i) * _cachedResult.BarycentricCoords()[i];
    }
    _cachedV = _cachedP1 - _cachedP2;
    ReduceVertices(_cachedResult.usedVertices);
    _cachedValidClosest = _cachedResult.IsValid();
    return _cachedValidClosest;*/
}

bool SimplexSolver::Closest(glm::vec3& v, const glm::vec3& origin)
{
    bool succes = UpdateClosest(origin);
    v = _cachedV;
    return succes;
}

void SimplexSolver::ComputePoints(glm::vec3& p1, glm::vec3& p2)
{
    UpdateClosest();
    p1 = _cachedP1;
    p2 = _cachedP2;
}