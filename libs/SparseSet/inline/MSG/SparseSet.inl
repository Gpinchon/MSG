#pragma once

namespace MSG {
template <typename Type, size_t Size>
constexpr SparseSet<Type, Size>::SparseSet() noexcept
{
    _sparse.fill(max_size());
}

template <typename Type, size_t Size>
inline SparseSet<Type, Size>::~SparseSet() noexcept(std::is_nothrow_invocable_v<decltype(&SparseSet::clear), SparseSet>)
{
    clear();
}

template <typename Type, size_t Size>
constexpr auto SparseSet<Type, Size>::max_size() const noexcept -> size_type
{
    return Size;
}

template <typename Type, size_t Size>
constexpr auto SparseSet<Type, Size>::size() const noexcept -> size_type
{
    return _size;
}

template <typename Type, size_t Size>
constexpr bool SparseSet<Type, Size>::empty() const noexcept
{
    return _size == 0;
}

template <typename Type, size_t Size>
constexpr bool SparseSet<Type, Size>::full() const noexcept
{
    return _size == max_size();
}

template <typename Type, size_t Size>
constexpr void SparseSet<Type, Size>::clear() noexcept(std::is_nothrow_invocable_v<decltype(&SparseSet::erase), SparseSet, size_type>)
{
    for (size_type index = 0; !empty(); ++index) {
        erase(index);
    }
}

template <typename Type, size_t Size>
constexpr auto SparseSet<Type, Size>::at(size_type a_Index) -> value_type&
{
    return _dense.at(_sparse.at(a_Index));
}

template <typename Type, size_t Size>
constexpr auto SparseSet<Type, Size>::at(size_type a_Index) const -> const value_type&
{
    return _dense.at(_sparse.at(a_Index));
}

template <typename Type, size_t Size>
constexpr auto SparseSet<Type, Size>::operator[](size_type a_Index) noexcept -> value_type&
{
    return _dense[_sparse[a_Index]];
}

template <typename Type, size_t Size>
constexpr auto SparseSet<Type, Size>::operator[](size_type a_Index) const noexcept -> const value_type&
{
    return _dense[_sparse[a_Index]];
}

template <typename Type, size_t Size>
template <typename... Args>
constexpr auto SparseSet<Type, Size>::insert(size_type a_Index, Args&&... a_Args) noexcept(std::is_nothrow_constructible_v<value_type, Args...> && std::is_nothrow_destructible_v<value_type>) -> value_type&
{
    if (contains(a_Index)) // just replace the element
    {
        auto& dense = _dense[_sparse[a_Index]];
        std::destroy_at((value_type*)dense.data);
        return *new (dense.data) value_type(std::forward<Args>(a_Args)...);
    }
    // Push new element back
    _sparse[a_Index]  = _size;
    auto& dense       = _dense.at(_size); // if full it should crash here
    dense.sparseIndex = a_Index;
    _size++;
    return *new (dense.data) value_type(std::forward<Args>(a_Args)...);
}

template <typename Type, size_t Size>
constexpr void SparseSet<Type, Size>::erase(size_type a_Index) noexcept(std::is_nothrow_destructible_v<value_type>)
{
    if (empty() || !contains(a_Index)) [[unlikely]]
        return;
    _size--;
    auto& currDense     = _dense[_sparse[a_Index]];
    auto& lastDense     = _dense[_size];
    size_type lastIndex = lastDense.sparseIndex;
    std::destroy_at((value_type*)currDense.data); // call current data's destructor
    std::memmove(currDense.data, lastDense.data, sizeof(value_type)); // crush current data with last data
    std::swap(lastDense.sparseIndex, currDense.sparseIndex);
    std::swap(_sparse[lastIndex], _sparse[a_Index]);
    _sparse[a_Index] = max_size();
}

template <typename Type, size_t Size>
constexpr bool SparseSet<Type, Size>::contains(size_type a_Index) const
{
    // if a_Index is out of bound we should crash here
    return _sparse.at(a_Index) != max_size();
}
}