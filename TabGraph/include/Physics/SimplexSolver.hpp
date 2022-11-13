#pragma once
#include <array>
#include <glm/glm.hpp>
#define UCHAR unsigned char
#define VERTA 0
#define VERTB 1
#define VERTC 2
#define VERTD 3

struct SimplexClosestSubSet {
    struct UsedVertices {
        bool a { false };
        bool b { false };
        bool c { false };
        bool d { false };
        void Reset()
        {
            a = false;
            b = false;
            c = false;
            d = false;
        }
    };
    bool degenerate { false };
    glm::vec3 closestPoint;
    UsedVertices usedVertices;
    void Reset()
    {
        SetBarycentricCoords();
        usedVertices.Reset();
        degenerate = false;
    }
    glm::vec4 BarycentricCoords() const { return _barycentricCoords; }
    void SetBarycentricCoords(float a = 0, float b = 0, float c = 0, float d = 0)
    {
        _barycentricCoords[0] = a;
        _barycentricCoords[1] = b;
        _barycentricCoords[2] = c;
        _barycentricCoords[3] = d;
    }
    bool IsValid()
    {
        return (BarycentricCoords()[0] >= 0) && (BarycentricCoords()[1] >= 0) && (BarycentricCoords()[2] >= 0) && (BarycentricCoords()[3] >= 0);
    }

private:
    glm::vec4 _barycentricCoords { -1 };
};

struct SimplexSolver {
    //std::vector<glm::vec3> Points;
    auto Size() { return _size; }
    auto& VertexW(UCHAR i) { return _verticesW.at(i); };
    auto& PointP(UCHAR i) { return _pointsP.at(i); };
    auto& PointQ(UCHAR i) { return _pointsQ.at(i); };
    void Reset()
    {
        _cachedValidClosest = false;
        _size = 0;
        _needsUpdate = true;
        _lastW = glm::vec3(std::numeric_limits<float>::max());
        _cachedResult.Reset();
    }
    void AddVertex(const glm::vec3& w, const glm::vec3& p, const glm::vec3& q)
    {
        _lastW = w;
        _needsUpdate = true;
        _verticesW.at(_size) = w;
        _pointsP.at(_size) = p;
        _pointsQ.at(_size) = q;
        _size++;
    };
    void RemoveVertex(UCHAR i)
    {
        assert(_size > 0);
        _size--;
        _verticesW.at(i) = _verticesW.at(_size);
        _pointsP.at(i) = _pointsP.at(_size);
        _pointsQ.at(i) = _pointsQ.at(_size);
    }
    void ReduceVertices(const SimplexClosestSubSet::UsedVertices& usedVertices)
    {
        if ((Size() >= 4) && (!usedVertices.d))
            RemoveVertex(3);
        if ((Size() >= 3) && (!usedVertices.c))
            RemoveVertex(2);
        if ((Size() >= 2) && (!usedVertices.b))
            RemoveVertex(1);
        if ((Size() >= 1) && (!usedVertices.a))
            RemoveVertex(0);
    }
    bool InSimplex(const glm::vec3& w)
    {
        for (auto i = 0; i < Size(); ++i) {
            if (w == VertexW(i))
                return true;
        }
        if (_lastW == w)
            return true;
        return false;
    }
    bool Closest(glm::vec3& v, const glm::vec3& origin = glm::vec3(0.f));
    bool UpdateClosest(const glm::vec3& origin);
    void ComputePoints(glm::vec3& p1, glm::vec3& p2);

private:
    bool _DoPoint(SimplexClosestSubSet& result);
    bool _DoPoint(SimplexClosestSubSet& result, const glm::vec3& a);
    bool _DoEdge(SimplexClosestSubSet& result, const glm::vec3& p);
    bool _DoEdge(SimplexClosestSubSet& result, const glm::vec3& p, const glm::vec3& a, const glm::vec3& b);
    bool _DoTriangle(SimplexClosestSubSet& result, const glm::vec3& p);
    bool _DoTriangle(SimplexClosestSubSet& result, const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
    bool _DoTetrahedron(SimplexClosestSubSet& result, const glm::vec3& p);
    bool _DoTetrahedron(SimplexClosestSubSet& result, const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d);
    UCHAR _size { 0 };
    std::array<glm::vec3, 4> _verticesW;
    std::array<glm::vec3, 4> _pointsP;
    std::array<glm::vec3, 4> _pointsQ;
    SimplexClosestSubSet _cachedResult;
    glm::vec3 _lastW { glm::vec3(std::numeric_limits<float>::max()) };
    glm::vec3 _cachedV;
    glm::vec3 _cachedP1;
    glm::vec3 _cachedP2;
    bool _cachedValidClosest { false };
    bool _needsUpdate { true };
};