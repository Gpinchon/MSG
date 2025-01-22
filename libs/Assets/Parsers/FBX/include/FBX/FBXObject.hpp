#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace FBX {
struct Object {
    virtual ~Object() = default;
    std::string Name() const;
    void SetName(const std::string&);
    virtual void Print() const;
    /** Searches for the corresponding object located at this id */
    static std::shared_ptr<Object> Get(int64_t id);
    /** Adds the object to specified id so it can be searched */
    static void Add(int64_t id, std::shared_ptr<Object>);

private:
    static std::map<int64_t, std::shared_ptr<Object>> s_objects;
    std::string _name { "" };
};
} // namespace FBX
