#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <sparsehash/sparse_hash_map>

#include <optional>
#include <queue>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
template <typename Type>
class BiMap {
public:
    using index_type    = uint64_t;
    using hash_type     = uint64_t;
    using value_type    = Type;
    using pair          = std::pair<index_type, const value_type&>;
    using queue_type    = std::queue<index_type>;
    using hash_map_type = google::sparse_hash_map<hash_type, index_type>;
    using storage_type  = std::vector<std::optional<value_type>>;
    struct const_iterator {
    private:
        struct ArrowHelper : pair {
            ArrowHelper(pair value);
            auto operator->() const { return this; }
        };
        BiMap<Type> const& _bimap;
        index_type _index;

    public:
        const_iterator(
            BiMap<Type> const& a_BiMap,
            const index_type& a_Index);
        const_iterator& operator=(const const_iterator& a_Other)
        {
            _index = a_Other._index;
            return *this;
        }
        const_iterator& operator++();
        const_iterator operator++(int);
        const_iterator& operator--();
        const_iterator operator--(int);
        const_iterator operator+(const index_type& a_Offset) const;
        inline pair operator*() const { return pair(_index, (_bimap._storage.begin() + _index)->value()); }
        inline ArrowHelper operator->() const { return **this; }
        inline bool operator==(const const_iterator& a_Other) const { return _index == a_Other._index; }
        inline bool operator!=(const const_iterator& a_Other) const { return _index != a_Other._index; }
    };
    BiMap();
    index_type operator[](const value_type& a_Value);
    Type const& operator[](const index_type& a_Index) const;
    index_type at(const value_type& a_Value) const;
    value_type const& at(const index_type& a_Index) const;
    bool contains(const index_type& a_Index) const;
    bool contains(const value_type& a_Value) const;
    void erase(const index_type& a_Index);
    void erase(const value_type& a_Value);
    const_iterator find(const index_type& a_Value) const;
    const_iterator find(const value_type& a_Value) const;
    const_iterator erase(const const_iterator& a_It);
    std::pair<index_type, bool> insert(const value_type& a_Value);
    void clear();
    size_t size() const;
    void reserve(const size_t& a_Size);
    const_iterator begin() const;
    const_iterator end() const;
    index_type first() const { return _first; }
    index_type last() const { return _last; }

private:
    index_type _first = 0;
    index_type _last  = 0;
    queue_type _freeIndice;
    hash_map_type _hashMap;
    storage_type _storage;
};
}

#include <MSG/BiMap.inl>
