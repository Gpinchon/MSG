#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <sparsehash/dense_hash_map>

#include <array>
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
private:
    using IndexType      = uint64_t;
    using HashType       = uint64_t;
    using KeyValuePair   = std::pair<IndexType, const Type&>;
    using IndexQueueType = std::queue<IndexType>;
    using HashMapType    = google::dense_hash_map<HashType, IndexType>;
    using StorageType    = std::vector<std::optional<Type>>;

public:
    struct const_iterator {
    private:
        struct ArrowHelper {
            ArrowHelper(KeyValuePair value);
            auto operator->() const { return &value; }
            KeyValuePair value;
        };
        StorageType::const_iterator _begin;
        StorageType::const_iterator _end;
        StorageType::const_iterator _itr;

    public:
        const_iterator(const StorageType::const_iterator& a_StorageBegItr, const StorageType::const_iterator& a_StorageEndItr, const StorageType::const_iterator& a_StorageItr);
        const_iterator& operator++();
        const_iterator operator++(int);
        inline KeyValuePair operator*() const { return KeyValuePair(std::distance(_begin, _itr), _itr->value()); }
        inline ArrowHelper operator->() const { return **this; }
        inline bool operator==(const const_iterator& a_Other) const { return _itr == a_Other._itr; }
        inline bool operator!=(const const_iterator& a_Other) const { return _itr != a_Other._itr; }
    };
    POBiMap();
    IndexType operator[](const Type& a_Value);
    Type const& operator[](const IndexType& a_Index) const;
    IndexType at(const Type& a_Value) const;
    Type const& at(const IndexType& a_Index) const;
    bool contains(const IndexType& a_Index) const;
    bool contains(const Type& a_Value) const;
    void erase(const IndexType& a_Index);
    void erase(const Type& a_Value);
    const_iterator erase(const const_iterator& a_It);
    std::pair<IndexType, bool> insert(const Type& a_Value);
    void clear();
    size_t size() const;
    void reserve(const size_t& a_Size);
    const_iterator begin() const;
    const_iterator end() const;

private:
    IndexQueueType _freeIndice;
    HashMapType _hashMap;
    StorageType _storage;
};

template <typename Type>
inline POBiMap<Type>::POBiMap()
{
    Type emptyValue;
    Type deletedValue;
    std::memset(&emptyValue, 255, sizeof(Type));
    std::memset(&deletedValue, 1, sizeof(Type));
    _hashMap.set_empty_key(std::hash<Type> {}(emptyValue));
    _hashMap.set_deleted_key(std::hash<Type> {}(deletedValue));
}

template <typename Type>
inline auto POBiMap<Type>::operator[](const Type& a_Value) -> IndexType
{
    auto itr = _hashMap.find(std::hash<Type> {}(a_Value));
    if (itr == _hashMap.end())
        return insert(a_Value).first;
    return itr->second;
}

template <typename Type>
inline auto POBiMap<Type>::operator[](const IndexType& a_Index) const -> Type const& { return at(a_Index); }
template <typename Type>
inline auto POBiMap<Type>::at(const Type& a_Value) const -> IndexType { return _hashMap.find(std::hash<Type> {}(a_Value))->second; }
template <typename Type>
inline auto POBiMap<Type>::at(const IndexType& a_Index) const -> Type const& { return _storage.at(a_Index).value(); }
template <typename Type>
inline bool POBiMap<Type>::contains(const IndexType& a_Index) const { return _storage.at(a_Index).has_value(); }
template <typename Type>
inline bool POBiMap<Type>::contains(const Type& a_Value) const { return _hashMap.find(std::hash<Type> {}(a_Value)) != _hashMap.end(); }
template <typename Type>
inline void POBiMap<Type>::erase(const IndexType& a_Index)
{
    if (contains(a_Index)) {
        assert(contains(*_storage.at(a_Index)));
        _freeIndice.push(a_Index);
        _hashMap.erase(std::hash<Type> {}(*_storage.at(a_Index)));
        _storage.at(a_Index).reset();
    }
}
template <typename Type>
inline void POBiMap<Type>::erase(const Type& a_Value) { erase(at(a_Value)); }
template <typename Type>
inline auto POBiMap<Type>::erase(const const_iterator& a_It) -> const_iterator
{
    assert(contains(a_It->first));
    erase(a_It->first);
    return ++const_iterator(a_It);
}
template <typename Type>
inline auto POBiMap<Type>::insert(const Type& a_Value) -> std::pair<IndexType, bool>
{
    auto hash = std::hash<Type> {}(a_Value);
    auto aItr = _hashMap.find(hash);
    if (aItr == _hashMap.end()) {
        IndexType newIndex;
        if (!_freeIndice.empty()) {
            newIndex = _freeIndice.back();
            _freeIndice.pop();
            _storage.at(newIndex).emplace(a_Value);
        } else {
            newIndex = _storage.size();
            _storage.emplace_back(a_Value);
        }
        _hashMap.insert({ hash, newIndex });
        return { newIndex, true };
    }
    return { aItr->second, false };
}
template <typename Type>
inline void POBiMap<Type>::clear() { *this = {}; }
template <typename Type>
inline size_t POBiMap<Type>::size() const
{
    return _hashMap.size();
}
template <typename Type>
inline void POBiMap<Type>::reserve(const size_t& a_Size)
{
    _hashMap.reserve(a_Size);
    _storage.reserve(a_Size);
}
template <typename Type>
inline auto POBiMap<Type>::begin() const -> const_iterator
{
    auto itr = _storage.begin();
    while (itr != _storage.end() && !itr->has_value())
        itr++;
    return { _storage.begin(), _storage.end(), itr };
}
template <typename Type>
auto POBiMap<Type>::end() const -> const_iterator { return { _storage.begin(), _storage.end(), _storage.end() }; }
template <typename Type>
inline POBiMap<Type>::const_iterator::ArrowHelper::ArrowHelper(KeyValuePair value)
    : value(value)
{
}
template <typename Type>
inline POBiMap<Type>::const_iterator::const_iterator(const StorageType::const_iterator& a_StorageBegItr, const StorageType::const_iterator& a_StorageEndItr, const StorageType::const_iterator& a_StorageItr)
    : _begin(a_StorageBegItr)
    , _end(a_StorageEndItr)
    , _itr(a_StorageItr)
{
}
template <typename Type>
POBiMap<Type>::const_iterator& POBiMap<Type>::const_iterator::operator++()
{
    _itr++;
    while (_itr != _end && !_itr->has_value())
        _itr++;
    if (_itr != _end) {
        auto ret = **this;
        assert(_itr->has_value());
    }
    return *this;
}
template <typename Type>
POBiMap<Type>::const_iterator POBiMap<Type>::const_iterator::operator++(int)
{
    const_iterator temp = *this;
    ++*this;
    return temp;
}
}
