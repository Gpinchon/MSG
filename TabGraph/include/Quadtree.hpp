/*
* @Author: gpinchon
* @Date:   2019-06-07 13:35:27
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:40
*/

#pragma once

#include <array> // for array
#include <glm/glm.hpp> // for s_vec2, glm::vec2, glm::vec2
#include <vector> // for vector

template <typename T>
class Quadtree {
public:
    Quadtree(glm::vec2 min, glm::vec2 max, int maxLevel = -1)
        : _min(min)
        , _max(max)
        , _mid((_min + _max) / 2.f)
        , _maxLevel(maxLevel) {};
    Quadtree() = delete;
    ~Quadtree() = default;
    Quadtree<T>* Get(int childIndex);
    bool Insert(T data, glm::vec2 min, glm::vec2 max);
    std::vector<T>& Data();
    std::vector<std::vector<T>*> GetAllData();
    glm::vec2 Min() { return _min; }
    glm::vec2 Max() { return _max; }
    glm::vec2 Mid() { return _mid; }
    int Level() { return _level; }

private:
    std::array<Quadtree<T>*, 4> _children { nullptr };
    std::vector<T> _data;
    glm::vec2 _min { 0, 0 };
    glm::vec2 _max { 0, 0 };
    glm::vec2 _mid { 0, 0 };
    int _maxLevel { -1 };
    int _level { 0 };
};

template <typename T>
inline bool Quadtree<T>::Insert(T data, glm::vec2 min, glm::vec2 max)
{
    int index = -1;
    bool top = false;
    bool bottom = false;
    bool left = false;
    bool right = false;
    bool inside = min.x >= _min.x && min.y >= _min.y
        && max.x <= _max.x && max.y <= _max.y;
    auto childMin = glm::vec2(-1);
    auto childMax = glm::vec2(-1);
    if (!inside)
        return false;
    if (min.y >= _mid.y) {
        top = true;
        childMin.y = _mid.y;
        childMax.y = _max.y;
    } else if (max.y <= _mid.y) {
        bottom = true;
        childMin.y = _min.y;
        childMax.y = _mid.y;
    }
    if (max.x <= _mid.x) {
        left = true;
        childMin.x = _min.x;
        childMax.x = _mid.x;
    } else if (min.x >= _mid.x) {
        right = true;
        childMin.x = _mid.x;
        childMax.x = _max.x;
    }

    if (top) {
        if (left)
            index = 0;
        else if (right)
            index = 1;
    } else if (bottom) {
        if (left)
            index = 2;
        else if (right)
            index = 3;
    }
    if (index == -1 || (_maxLevel > -1 && _level >= _maxLevel)) {
        _data.push_back(data);
        return true;
    }
    if (_children.at(index) == nullptr) {
        _children.at(index) = new Quadtree<T>(childMin, childMax, _maxLevel);
        _children.at(index)->_level = _level + 1;
    }
    return _children.at(index)->Insert(data, min, max);
}

template <typename T>
std::vector<std::vector<T>*> Quadtree<T>::GetAllData()
{
    std::vector<std::vector<T>*> data;
    data.push_back(&_data);
    for (const auto& child : _children) {
        if (child) {
            for (auto& childData : child->GetAllData())
                data.push_back(childData);
        }
    }
    return data;
}

template <typename T>
inline Quadtree<T>* Quadtree<T>::Get(int childIndex)
{
    return _children.at(childIndex);
}

template <typename T>
inline std::vector<T>& Quadtree<T>::Data()
{
    return _data;
}