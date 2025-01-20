#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/POBiMap.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG::SG {
template <typename Type>
class POStorage : POBiMap<Type> {
public:
    using size_type      = size_t;
    using ref_type       = uint64_t;
    using value_type     = POBiMap<Type>::value_type;
    using index_type     = POBiMap<Type>::index_type;
    using const_iterator = POBiMap<Type>::const_iterator;
    ref_type refCount(const index_type& a_Index) const { return _refCounts.at(a_Index); }
    ref_type refCount(const value_type& a_Value) const { return refCount(at(a_Value)); }
    bool contains(const index_type& a_Index) const { return POBiMap<Type>::contains(a_Index); }
    bool contains(const value_type& a_Value) const { return POBiMap<Type>::contains(a_Value); }
    std::pair<index_type, bool> insert(const value_type& a_Value)
    {
        auto ret = POBiMap<Type>::insert(a_Value);
        _refCounts.resize(POBiMap<Type>::last() + 1);
        return ret;
    }
    const value_type& ref(const index_type& a_Index)
    {
        assert(contains(a_Index));
        _refCounts.at(a_Index)++;
        return at(a_Index);
    }
    ///@return true if element was deleted
    bool unref(const index_type& a_Index)
    {
        assert(refCount(a_Index) > 0);
        _refCounts.at(a_Index)--;
        if (refCount(a_Index) == 0) {
            POBiMap<Type>::erase(a_Index);
            _refCounts.resize(POBiMap<Type>::last() + 1);
            return true;
        }
        return false;
    };
    ///@return true if element was deleted
    bool unref(const value_type& a_Value) { return unref(at(a_Value)); }
    const_iterator find(const index_type& a_Index) const { return POBiMap<Type>::find(a_Index); }
    const_iterator find(const value_type& a_Value) const { return POBiMap<Type>::find(a_Value); }
    const_iterator begin() const { return POBiMap<Type>::begin(); }
    const_iterator end() const { return POBiMap<Type>::end(); }
    const value_type& at(const index_type& a_Index) const { return POBiMap<Type>::at(a_Index); }
    const value_type& operator[](const index_type& a_Index) const { return POBiMap<Type>::at(a_Index); }
    index_type at(const value_type& a_Value) const { return POBiMap<Type>::at(a_Value); }
    index_type operator[](const value_type& a_Value) const { return POBiMap<Type>::at(a_Value); }
    size_type size() const { return POBiMap<Type>::size(); }
    void reserve(const size_type& a_Size)
    {
        _refCounts.reserve(a_Size);
        POBiMap<Type>::reserve(a_Size);
    };

private:
    std::vector<ref_type> _refCounts;
};
}
