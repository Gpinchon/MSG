#pragma once
namespace Msg {
template <typename Type>
inline BiMap<Type>::BiMap()
{
    value_type deletedValue;
    std::memset(&deletedValue, 255, sizeof(value_type));
    _hashMap.set_deleted_key(std::hash<value_type> {}(deletedValue));
}

template <typename Type>
inline auto BiMap<Type>::operator[](const Type& a_Value) -> index_type
{
    auto itr = _hashMap.find(std::hash<value_type> {}(a_Value));
    if (itr == _hashMap.end())
        return insert(a_Value).first;
    return itr->second;
}

template <typename Type>
inline auto BiMap<Type>::operator[](const index_type& a_Index) const -> value_type const& { return at(a_Index); }
template <typename Type>
inline auto BiMap<Type>::at(const Type& a_Value) const -> index_type { return _hashMap.find(std::hash<value_type> {}(a_Value))->second; }
template <typename Type>
inline auto BiMap<Type>::at(const index_type& a_Index) const -> value_type const& { return _storage.at(a_Index).value(); }
template <typename Type>
inline bool BiMap<Type>::contains(const index_type& a_Index) const { return _storage.at(a_Index).has_value(); }
template <typename Type>
inline bool BiMap<Type>::contains(const Type& a_Value) const { return _hashMap.find(std::hash<value_type> {}(a_Value)) != _hashMap.end(); }
template <typename Type>
inline void BiMap<Type>::erase(const index_type& a_Index)
{
    auto itr = find(a_Index);
    if (itr != end()) {
        assert(contains(*_storage.at(a_Index)));
        erase(itr);
    }
}
template <typename Type>
inline void BiMap<Type>::erase(const Type& a_Value) { erase(at(a_Value)); }
template <typename Type>
inline auto BiMap<Type>::find(const index_type& a_Index) const -> const_iterator
{
    if (_storage.at(a_Index).has_value())
        return { *this, a_Index };
    else
        return end();
}
template <typename Type>
inline auto BiMap<Type>::find(const Type& a_Value) const -> const_iterator
{
    auto hashItr = _hashMap.find(std::hash<value_type> {}(a_Value));
    if (hashItr != _hashMap.end())
        return { *this, hashItr->second };
    else
        return end();
}
template <typename Type>
inline auto BiMap<Type>::erase(const const_iterator& a_It) -> const_iterator
{
    assert(contains(a_It->first));
    index_type index = a_It->first;
    _freeIndice.push(index);
    _hashMap.erase(std::hash<value_type> {}(a_It->second));
    _storage.at(index).reset();
    if (_first == index) // get the next item
        _first = (++const_iterator(a_It))->first;
    if (_last == index) { // get the previous item
        _last = (--const_iterator(a_It))->first;
        return end(); // we just removed the last item
    }
    return ++const_iterator(a_It);
}
template <typename Type>
inline auto BiMap<Type>::insert(const Type& a_Value) -> std::pair<index_type, bool>
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
inline void BiMap<Type>::clear() { *this = {}; }
template <typename Type>
inline size_t BiMap<Type>::size() const { return _hashMap.size(); }
template <typename Type>
inline void BiMap<Type>::reserve(const size_t& a_Size)
{
    _hashMap.reserve(a_Size);
    _storage.reserve(a_Size);
}
template <typename Type>
inline auto BiMap<Type>::begin() const -> const_iterator
{
    return { *this, _first };
}
template <typename Type>
auto BiMap<Type>::end() const -> const_iterator
{
    return { *this, _last + 1 };
}
template <typename Type>
inline BiMap<Type>::const_iterator::ArrowHelper::ArrowHelper(pair value)
    : pair(value)
{
}
template <typename Type>
inline BiMap<Type>::const_iterator::const_iterator(
    BiMap<Type> const& a_BiMap,
    const uint64_t& a_Index)
    : _bimap(a_BiMap)
    , _index(a_Index)
{
}
template <typename Type>
BiMap<Type>::const_iterator& BiMap<Type>::const_iterator::operator++()
{
    assert(_index <= _bimap.last());
    _index++;
    while (_index <= _bimap.last() && !(_bimap._storage.begin() + _index)->has_value())
        _index++;
    if (_index <= _bimap.last())
        assert((_bimap._storage.begin() + _index)->has_value());
    return *this;
}
template <typename Type>
BiMap<Type>::const_iterator BiMap<Type>::const_iterator::operator++(int)
{
    const_iterator temp = *this;
    ++*this;
    return temp;
}
template <typename Type>
inline auto BiMap<Type>::const_iterator::operator--() -> const_iterator&
{
    _index--;
    while (*this != _bimap.begin() && !(_bimap._storage.begin() + _index)->has_value())
        _index--;
    return *this;
}
template <typename Type>
inline auto BiMap<Type>::const_iterator::operator--(int) -> const_iterator
{
    const_iterator temp = *this;
    --*this;
    return temp;
}
template <typename Type>
inline auto BiMap<Type>::const_iterator::operator+(const index_type& a_Offset) const -> const_iterator
{
    const_iterator temp = *this;
    temp._index += a_Offset;
    return temp;
}
}
