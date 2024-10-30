#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <sparsehash/dense_hash_map>

#include <queue>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
template <typename Type>
class POBiMap {
public:
    POBiMap();
    using IndexType = uint64_t;
    using MapAType  = google::dense_hash_map<Type, IndexType>;
    using MapBType  = google::dense_hash_map<IndexType, Type>;
    IndexType operator[](const Type& a_Value);
    Type const& operator[](const IndexType& a_Index) const;
    IndexType at(const Type& a_Value) const;
    Type const& at(const IndexType& a_Index) const;
    bool contains(const IndexType& a_Index) const;
    bool contains(const Type& a_Value) const;
    void erase(const IndexType& a_Index);
    void erase(const Type& a_Value);
    MapBType::const_iterator erase(const MapBType::const_iterator& a_It);
    std::pair<IndexType, bool> insert(const Type& a_Value);
    void clear();
    size_t size() const;
    void reserve(const size_t& a_Size);
    MapBType::const_iterator begin() const;
    MapBType::const_iterator end() const;

private:
    std::queue<IndexType> freeIndice;
    IndexType id = 0;
    MapAType mapA;
    MapBType mapB;
};

template <typename Type>
POBiMap<Type>::POBiMap()
{
    Type emptyValue;
    Type deletedValue;
    IndexType emptyIndex   = std::numeric_limits<IndexType>::max() - 1;
    IndexType deletedIndex = std::numeric_limits<IndexType>::max();
    std::memset(&emptyValue, 1, sizeof(Type));
    std::memset(&deletedValue, -1, sizeof(Type));
    mapA.set_empty_key(emptyValue);
    mapB.set_empty_key(emptyIndex);
    mapA.set_deleted_key(deletedValue);
    mapB.set_deleted_key(deletedIndex);
}

template <typename Type>
auto POBiMap<Type>::operator[](const Type& a_Value) -> IndexType
{
    auto itr = mapA.find(a_Value);
    if (itr == mapA.end())
        return insert(a_Value).first;
    return itr->second;
}

template <typename Type>
auto POBiMap<Type>::operator[](const IndexType& a_Index) const -> Type const& { return at(a_Index); }
template <typename Type>
auto POBiMap<Type>::at(const Type& a_Value) const -> IndexType { return mapA.find(a_Value)->second; }
template <typename Type>
auto POBiMap<Type>::at(const IndexType& a_Index) const -> Type const& { return mapB.find(a_Index)->second; }
template <typename Type>
bool POBiMap<Type>::contains(const IndexType& a_Index) const { return mapB.find(a_Index) != mapB.end(); }
template <typename Type>
bool POBiMap<Type>::contains(const Type& a_Value) const { return mapA.find(a_Value) != mapA.end(); }
template <typename Type>
void POBiMap<Type>::erase(const IndexType& a_Index)
{
    const auto& value = at(a_Index);
    if (contains(a_Index))
        assert(contains(value));
    mapA.erase(value);
    mapB.erase(a_Index);
    freeIndice.push(a_Index);
    assert(mapA.size() == mapB.size());
}
template <typename Type>
void POBiMap<Type>::erase(const Type& a_Value) { erase(at(a_Value)); }
template <typename Type>
auto POBiMap<Type>::erase(const MapBType::const_iterator& a_It) -> MapBType::const_iterator
{
    freeIndice.push(a_It->first);
    mapA.erase(a_It->second);
    return mapB.erase(a_It);
}
template <typename Type>
auto POBiMap<Type>::insert(const Type& a_Value) -> std::pair<IndexType, bool>
{
    auto aItr = mapA.find(a_Value);
    if (aItr == mapA.end()) {
        IndexType newIndice;
        if (!freeIndice.empty()) {
            newIndice = freeIndice.back();
            freeIndice.pop();
        } else {
            newIndice = id;
            id++;
        }
        mapA.insert({ a_Value, newIndice });
        mapB.insert({ newIndice, a_Value });
        assert(mapA.size() == mapB.size());
        return { newIndice, true };
    }
    return { aItr->second, false };
}
template <typename Type>
void POBiMap<Type>::clear() { *this = {}; }
template <typename Type>
size_t POBiMap<Type>::size() const
{
    assert(mapA.size() == mapB.size());
    return mapA.size();
}
template <typename Type>
void POBiMap<Type>::reserve(const size_t& a_Size)
{
    mapA.reserve(a_Size);
    mapB.reserve(a_Size);
}
template <typename Type>
auto POBiMap<Type>::begin() const -> MapBType::const_iterator { return mapB.begin(); }
template <typename Type>
auto POBiMap<Type>::end() const -> MapBType::const_iterator { return mapB.end(); }
}
