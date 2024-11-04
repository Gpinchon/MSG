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
namespace TabGraph::SG {
template <typename Type>
class POBiMap {
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
        storage_type::const_iterator _beginItr;
        index_type _begin;
        index_type _end;
        index_type _index;

    public:
        const_iterator(
            const storage_type::const_iterator& a_StorageBegItr,
            const index_type& a_First,
            const index_type& a_Last,
            const index_type& a_Index);
        const_iterator& operator++();
        const_iterator operator++(int);
        const_iterator& operator--();
        const_iterator operator--(int);
        const_iterator operator+(const index_type& a_Offset) const;
        inline pair operator*() const { return pair(_index, (_beginItr + _index)->value()); }
        inline ArrowHelper operator->() const { return **this; }
        inline bool operator==(const const_iterator& a_Other) const { return _index == a_Other._index; }
        inline bool operator!=(const const_iterator& a_Other) const { return _index != a_Other._index; }
    };
    POBiMap();
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

template <typename Type>
inline POBiMap<Type>::POBiMap()
{
    value_type deletedValue;
    std::memset(&deletedValue, 255, sizeof(value_type));
    _hashMap.set_deleted_key(std::hash<value_type> {}(deletedValue));
}

template <typename Type>
inline auto POBiMap<Type>::operator[](const Type& a_Value) -> index_type
{
    auto itr = _hashMap.find(std::hash<value_type> {}(a_Value));
    if (itr == _hashMap.end())
        return insert(a_Value).first;
    return itr->second;
}

template <typename Type>
inline auto POBiMap<Type>::operator[](const index_type& a_Index) const -> value_type const& { return at(a_Index); }
template <typename Type>
inline auto POBiMap<Type>::at(const Type& a_Value) const -> index_type { return _hashMap.find(std::hash<value_type> {}(a_Value))->second; }
template <typename Type>
inline auto POBiMap<Type>::at(const index_type& a_Index) const -> value_type const& { return _storage.at(a_Index).value(); }
template <typename Type>
inline bool POBiMap<Type>::contains(const index_type& a_Index) const { return _storage.at(a_Index).has_value(); }
template <typename Type>
inline bool POBiMap<Type>::contains(const Type& a_Value) const { return _hashMap.find(std::hash<value_type> {}(a_Value)) != _hashMap.end(); }
template <typename Type>
inline void POBiMap<Type>::erase(const index_type& a_Index)
{
    auto itr = find(a_Index);
    if (itr != end()) {
        assert(contains(*_storage.at(a_Index)));
        erase(itr);
    }
}
template <typename Type>
inline void POBiMap<Type>::erase(const Type& a_Value) { erase(at(a_Value)); }
template <typename Type>
inline auto POBiMap<Type>::find(const index_type& a_Index) const -> const_iterator
{
    if (_storage.at(a_Index).has_value())
        return { _storage.begin(), _first, _last + 1, a_Index };
    else
        return end();
}
template <typename Type>
inline auto POBiMap<Type>::find(const Type& a_Value) const -> const_iterator
{
    auto hashItr = _hashMap.find(std::hash<value_type> {}(a_Value));
    if (hashItr != _hashMap.end())
        return { _storage.begin(), _first, _last + 1, hashItr->second };
    else
        return end();
}
template <typename Type>
inline auto POBiMap<Type>::erase(const const_iterator& a_It) -> const_iterator
{
    assert(contains(a_It->first));
    auto& index = a_It->first;
    auto& value = a_It->second;
    if (_first == index) // get the next item
        _first = (++const_iterator(a_It))->first;
    if (_last == index) // get the previous item
        _last = (--const_iterator(a_It))->first;
    _freeIndice.push(index);
    _hashMap.erase(std::hash<value_type> {}(value));
    _storage.at(index).reset();
    return ++const_iterator(a_It);
}
template <typename Type>
inline auto POBiMap<Type>::insert(const Type& a_Value) -> std::pair<index_type, bool>
{
    auto hash = std::hash<value_type> {}(a_Value);
    auto aItr = _hashMap.find(hash);
    if (aItr == _hashMap.end()) {
        index_type newIndex;
        if (!_freeIndice.empty()) {
            newIndex = _freeIndice.front();
            _freeIndice.pop();
            _storage.at(newIndex).emplace(a_Value);
        } else {
            newIndex = _storage.size();
            _storage.emplace_back(a_Value);
        }
        _hashMap.insert({ hash, newIndex });
        _first = std::min(_first, newIndex);
        _last  = std::max(_last, newIndex);
        return { newIndex, true };
    }
    return { aItr->second, false };
}
template <typename Type>
inline void POBiMap<Type>::clear() { *this = {}; }
template <typename Type>
inline size_t POBiMap<Type>::size() const { return _hashMap.size(); }
template <typename Type>
inline void POBiMap<Type>::reserve(const size_t& a_Size)
{
    _hashMap.reserve(a_Size);
    _storage.reserve(a_Size);
}
template <typename Type>
inline auto POBiMap<Type>::begin() const -> const_iterator
{
    return { _storage.begin(), _first, _last + 1, _first };
}
template <typename Type>
auto POBiMap<Type>::end() const -> const_iterator
{
    return { _storage.begin(), _first, _last + 1, _last + 1 };
}
template <typename Type>
inline POBiMap<Type>::const_iterator::ArrowHelper::ArrowHelper(pair value)
    : pair(value)
{
}
template <typename Type>
inline POBiMap<Type>::const_iterator::const_iterator(
    const storage_type::const_iterator& a_StorageBegItr,
    const uint64_t& a_First,
    const uint64_t& a_Last,
    const uint64_t& a_Index)
    : _beginItr(a_StorageBegItr)
    , _begin(a_First)
    , _end(a_Last)
    , _index(a_Index)
{
}
template <typename Type>
POBiMap<Type>::const_iterator& POBiMap<Type>::const_iterator::operator++()
{
    _index++;
    while (_index != _end && !(_beginItr + _index)->has_value())
        _index++;
    if (_index != _end)
        assert((_beginItr + _index)->has_value());
    return *this;
}
template <typename Type>
POBiMap<Type>::const_iterator POBiMap<Type>::const_iterator::operator++(int)
{
    const_iterator temp = *this;
    ++*this;
    return temp;
}
template <typename Type>
inline auto POBiMap<Type>::const_iterator::operator--() -> const_iterator&
{
    _index--;
    while (_index != _begin && !(_beginItr + _index)->has_value())
        _index--;
    return *this;
}
template <typename Type>
inline auto POBiMap<Type>::const_iterator::operator--(int) -> const_iterator
{
    const_iterator temp = *this;
    --*this;
    return temp;
}
template <typename Type>
inline auto POBiMap<Type>::const_iterator::operator+(const index_type& a_Offset) const -> const_iterator
{
    const_iterator temp = *this;
    temp._index += a_Offset;
    return temp;
}
}
