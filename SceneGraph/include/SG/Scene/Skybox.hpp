/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:26
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Property.hpp>
#include <SG/Shape/Shape.hpp>

#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Image;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Skybox : public Inherit<Shape, Skybox> {
public:
    PROPERTY(std::shared_ptr<Image>, Image, nullptr);

public:
    Skybox(const std::string& a_Name) : Inherit(a_Name) {}
    Skybox(const std::string& a_Name, std::shared_ptr<Image> a_Image)
        : Skybox(a_Name)
    {
        SetImage(a_Image);
    }
};
}

