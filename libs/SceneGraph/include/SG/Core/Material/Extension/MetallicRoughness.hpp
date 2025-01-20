/*
 * @Author: gpinchon
 * @Date:   2020-10-01 15:30:47
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-02-18 22:49:43
 */
#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Material/Extension/Extension.hpp>
#include <SG/Core/Material/TextureInfo.hpp>

#include <glm/vec4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::SG {
struct MetallicRoughnessExtension : MaterialExtension {
    glm::vec4 colorFactor                = { 1, 1, 1, 1 };
    TextureInfo colorTexture             = {};
    float metallicFactor                 = { 1 };
    float roughnessFactor                = { 1 };
    TextureInfo metallicRoughnessTexture = {};
};
}
