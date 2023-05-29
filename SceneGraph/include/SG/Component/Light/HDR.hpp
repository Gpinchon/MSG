/*
 * @Author: gpinchon
 * @Date:   2021-03-14 22:12:40
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-05-04 20:02:25
 */
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Image;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
struct LightHDR {
    std::shared_ptr<Image> image; // an equirectangular image to be used as light
};
}
