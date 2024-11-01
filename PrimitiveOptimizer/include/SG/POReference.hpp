#pragma once
#define POREF_USE_SPARSE_HASH
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#ifdef POREF_USE_SPARSE_HASH
#include <sparsehash/sparse_hash_set>
#else
#include <unordered_set>
#endif

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class POReference {
public:
    POReference()
    {
#ifdef POREF_USE_SPARSE_HASH
        pairs.set_deleted_key(std::numeric_limits<uint64_t>::max());
        triangles.set_deleted_key(std::numeric_limits<uint64_t>::max());
#endif
        pairs.reserve(128);
        triangles.reserve(128);
    }
    bool ContainsPair(const uint64_t& a_Index) const { return pairs.find(a_Index) != pairs.end(); }
    bool ContainsTriangle(const uint64_t& a_Index) const { return triangles.find(a_Index) != triangles.end(); }
    // move a_Other to this reference list
    void operator<<(POReference& a_Other)
    {
#ifdef POREF_USE_SPARSE_HASH
        pairs.insert(a_Other.pairs.begin(), a_Other.pairs.end());
        triangles.insert(a_Other.triangles.begin(), a_Other.triangles.end());
        a_Other.pairs.clear();
        a_Other.triangles.clear();
#else
        pairs.merge(std::move(a_Other.pairs));
        triangles.merge(std::move(a_Other.triangles));
#endif
    }
#ifdef POREF_USE_SPARSE_HASH
    google::sparse_hash_set<uint64_t> pairs;
    google::sparse_hash_set<uint64_t> triangles;
#else
    std::unordered_set<uint64_t> pairs;
    std::unordered_set<uint64_t> triangles;
#endif
};
}
