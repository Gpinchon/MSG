/*
* @Author: gpinchon
* @Date:   2020-08-27 18:48:20
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:43:12
*/

#ifdef USE_GDAL

#include "Terrain/Terrain_GDAL.hpp" // for Terrain
#include "Debug.hpp" // for debugLog
#include "Material.hpp" // for Material
#include "Mesh/Geometry.hpp"
#include "Mesh/Mesh.hpp" // for Mesh
#include "Node.hpp" // for Node
#include "Quadtree.hpp" // for Quadtree
#include "Texture/Texture.hpp" // for Texture
#include "Texture/TextureParser.hpp" // for TextureParser

#include "cpl_conv.h" // for CPLMalloc, CPLFree
#include "cpl_error.h" // for CE_None
#include "gdal.h" // for GDALAllRegister, GDALClose, GDALOpen
#include <GL/glew.h> // for GL_COMPRESSED_RED, GL_FLOAT, GL_RED
#include <array> // for array
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <gdal_priv.h> // for GDALRasterBand, GDALDataset
#include <glm/glm.hpp> // for s_vec3, glm::vec2, s_vec2, glm::vec3, glm::vec3
#include <iostream> // for operator<<, basic_ostream<>::__ostream...
#include <limits> // for numeric_limits
#include <math.h> // for abs
#include <memory> // for shared_ptr, __shared_ptr_access, alloc...
#include <stdexcept> // for runtime_error
#include <stdint.h> // for uint32_t
#include <stdlib.h> // for abs
#include <string> // for operator+, string
#include <vector> // for vector

Terrain::Terrain(const std::string& name)
    : Mesh(name)
{
}

void Subdivide(Quadtree<std::array<glm::vec3, 4>>* tree,
    std::shared_ptr<Texture> texture, glm::vec3 scale)
{
    if (tree == nullptr)
        return;

    bool subdivided = false;
    auto scale2 = glm::vec2(scale.x, scale.z);
    glm::vec2 pixelSize = glm::vec2(texture->size()) / scale2;
    // std::cout << "PixelSize " << pixelSize.x << ' ' << pixelSize.y <<
    // std::endl;
    for (auto index = 0u; index < tree->Data().size(); index++) {
        if (subdivided)
            index = 0;
        subdivided = false;
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float medY = 0;
        float delta = 0;
        for (auto v : tree->Data().at(index)) {
            minY = v.y < minY ? v.y : minY;
            maxY = v.y > maxY ? v.y : maxY;
            medY += v.y * 0.25;
        }
        delta = (maxY - minY) / maxY;
        if (delta < 0.1) {
            continue;
        }
        auto v0 = tree->Data().at(index).at(0);
        auto v1 = tree->Data().at(index).at(1);
        auto v2 = tree->Data().at(index).at(2);
        auto v3 = tree->Data().at(index).at(3);
        if (glm::abs(v0.x - v2.x) <= pixelSize.x || glm::abs(v0.z - v2.z) <= pixelSize.y)
            continue;
        auto mid = (v0 + v2) / 2.f;
        std::array<glm::vec2, 5> uvs {
            glm::vec2(mid.x, v2.z), glm::vec2(v2.x, mid.z),
            glm::vec2(mid.x, v0.z), glm::vec2(v0.x, mid.z),
            glm::vec2(mid.x, mid.z)
        };
        std::array<float, 5> samples {
            (v1.y + v2.y) / 2.f, (v2.y + v3.y) / 2.f, (v3.y + v0.y) / 2.f,
            (v0.y + v1.y) / 2.f,
            ((float*)texture->texelfetch(((uvs.at(4) / scale2) + 0.5f) * texture->size()))[0] * scale.y
        };
        if (std::equal(samples.begin() + 1, samples.end(), samples.begin()))
            continue;
        auto v4 = glm::vec3(uvs.at(0).x, samples.at(0), uvs.at(0).y);
        auto v5 = glm::vec3(uvs.at(1).x, samples.at(1), uvs.at(1).y);
        auto v6 = glm::vec3(uvs.at(2).x, samples.at(2), uvs.at(2).y);
        auto v7 = glm::vec3(uvs.at(3).x, samples.at(3), uvs.at(3).y);
        auto v8 = glm::vec3(uvs.at(4).x, samples.at(4), uvs.at(4).y);
        // std::cout <<
        // "----------------------------------------------------------" << '\n';
        // std::cout << v4.x << ' ' << v4.y << ' ' << v4.z << '\n';
        // std::cout << v5.x << ' ' << v5.y << ' ' << v5.z << '\n';
        // std::cout << v6.x << ' ' << v6.y << ' ' << v6.z << '\n';
        // std::cout << v7.x << ' ' << v7.y << ' ' << v7.z << '\n';
        // std::cout << v8.x << ' ' << v8.y << ' ' << v8.z << '\n';
        // std::cout <<
        // "----------------------------------------------------------" <<
        // std::endl;
        tree->Data().erase(tree->Data().begin() + index);
        auto patch0 = std::array<glm::vec3, 4> { v0, v7, v8, v6 };
        auto patch1 = std::array<glm::vec3, 4> { v7, v1, v4, v8 };
        auto patch2 = std::array<glm::vec3, 4> { v8, v4, v2, v5 };
        auto patch3 = std::array<glm::vec3, 4> { v6, v8, v5, v3 };
        tree->Insert(patch0, glm::vec2(v0.x, v0.z), glm::vec2(v8.x, v8.z));
        tree->Insert(patch1, glm::vec2(v7.x, v7.z), glm::vec2(v4.x, v4.z));
        tree->Insert(patch2, glm::vec2(v8.x, v8.z), glm::vec2(v2.x, v2.z));
        tree->Insert(patch3, glm::vec2(v6.x, v6.z), glm::vec2(v5.x, v5.z));
        subdivided = true;
    }
    for (auto i = 0; i < 4; i++) {
        Subdivide(tree->Get(i), texture, scale);
    }
}

glm::vec3 TriangleNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
    auto u = glm::normalize(p2 - p1);
    auto v = glm::normalize(p3 - p1);
    return glm::cross(u, v);
}

std::shared_ptr<Terrain> Terrain::Create(const std::string& name,
    glm::ivec2 resolution, glm::vec3 scale,
    std::shared_ptr<Texture> texture)
{
    resolution.x = resolution.x > texture->size().x ? texture->size().x : resolution.x;
    resolution.y = resolution.y > texture->size().y ? texture->size().y : resolution.y;
    auto terrain = tools::make_sharedr<Terrain>(name);
    terrain->_terrainData = texture;
    terrain->_terrainResolution = resolution;
    terrain->_terrainSize = scale;
    Mesh::Add(terrain);
    std::vector<glm::vec3> v;
    // std::vector<glm::vec2> vt;
    // std::vector<glm::vec<3, uint8_t>> vn;

    std::vector<uint32_t> i;
    // float minDepth = std::numeric_limits<float>::max();
    // float maxDepth = std::numeric_limits<float>::lowest();
    std::cout << scale.x / 2.f << " " << scale.z / 2.f << std::endl;
    Quadtree<std::array<glm::vec3, 4>> quadTree(
        glm::vec2(-scale.x / 2.f, -scale.z / 2.f),
        glm::vec2(scale.x / 2.f, scale.z / 2.f));
    for (auto y = 0.f; y < resolution.y; y++) {
        for (auto x = 0.f; x < resolution.x; x++) {
            auto uv = glm::vec2(x / resolution.x, y / resolution.y);
            auto depth = 0.f;
            if (texture) {
                glm::vec2 texUV;
                texUV.x = uv.x * texture->size().x;
                texUV.y = uv.y * texture->size().y;
                depth = ((float*)texture->texelfetch(texUV))[0];

                // minDepth = depth < minDepth ? depth : minDepth;
                // maxDepth = depth > maxDepth ? depth : maxDepth;
            }
            // vg->vt.at(uint32_t(x + y * resolution.x)) = uv;
            // auto &v3 = vg->v.at(uint32_t(x + y * resolution.x));
            auto v3 = glm::vec3(uv.x * scale.x - scale.x / 2.f, depth * scale.y,
                uv.y * scale.z - scale.z / 2.f);
            v.push_back(v3);
            if (x < resolution.x - 1 && y < resolution.y - 1) {
                i.push_back(uint32_t(x + y * resolution.x));
                i.push_back(uint32_t(x + (y + 1) * resolution.x));
                i.push_back(uint32_t((x + 1) + (y + 1) * resolution.x));
                i.push_back(uint32_t((x + 1) + y * resolution.x));
            }
        }
    }
    for (auto index = 0u; index < i.size() - 4; index += 4) {
        auto v0 = v.at(i.at(index + 0));
        auto v1 = v.at(i.at(index + 1));
        auto v2 = v.at(i.at(index + 2));
        auto v3 = v.at(i.at(index + 3));
        auto patch = std::array<glm::vec3, 4> { v0, v1, v2, v3 };
        quadTree.Insert(patch, glm::vec2(v0.x, v0.z), glm::vec2(v2.x, v2.z));
    }
    std::cout << std::endl;
    Subdivide(&quadTree, texture, scale);
    auto vg = Component::Create<Geometry>(name + "_Geometry");
    // auto medZ = (maxDepth + minDepth) / 2.f * scale.y;
    for (auto& data : quadTree.GetAllData()) {
        for (auto& patch : *data) {
            auto n0 = TriangleNormal(patch.at(0), patch.at(1), patch.at(2));
            auto n1 = TriangleNormal(patch.at(2), patch.at(3), patch.at(0));
            n0 = (n0 + 1.f) / 2.f * 255.f;
            n1 = (n1 + 1.f) / 2.f * 255.f;
            auto n01 = (n0 + n1) / 2.f;
            auto cn0 = (CVEC4) { GLubyte(n0.x), GLubyte(n0.y), GLubyte(n0.z), 1 };
            auto cn1 = (CVEC4) { GLubyte(n1.x), GLubyte(n1.y), GLubyte(n1.z), 1 };
            auto cn01 = (CVEC4) { GLubyte(n01.x), GLubyte(n01.y), GLubyte(n01.z), 1 };
            vg->v.push_back(patch.at(0));
            vg->vn.push_back(cn01);
            vg->v.push_back(patch.at(1));
            vg->vn.push_back(cn0);
            vg->v.push_back(patch.at(2));
            vg->vn.push_back(cn01);

            vg->v.push_back(patch.at(2));
            vg->vn.push_back(cn01);
            vg->v.push_back(patch.at(3));
            vg->vn.push_back(cn1);
            vg->v.push_back(patch.at(0));
            vg->vn.push_back(cn01);

            /*for (auto& v : patch) {
                v.y -= medZ;
                std::cout << v.x << ' ' << v.y << ' ' << v.z << '\n';
                vg->v.push_back(v);
            }*/
        }
    }

    /*
  for (auto index = 0u; index * 3 < vg->i.size(); index++)
  {
      auto i0 = vg->i.at(index * 3 + 0);
      auto i1 = vg->i.at(index * 3 + 1);
      auto i2 = vg->i.at(index * 3 + 2);
      auto v0 = vg->v.at(i0);
      auto v1 = vg->v.at(i1);
      auto v2 = vg->v.at(i2);
      auto &n0 = vg->vn.at(i0);
      auto &n1 = vg->vn.at(i1);
      auto &n2 = vg->vn.at(i2);

      glm::vec3 N0 = glm::vec3((n0.x / 255.f) * 2 - 1, (n0.y / 255.f) * 2 - 1,
  (n0.z / 255.f) * 2 - 1); glm::vec3 N1 = glm::vec3((n1.x / 255.f) * 2 - 1,
  (n1.y / 255.f) * 2 - 1, (n1.z / 255.f) * 2 - 1); glm::vec3 N2 =
  glm::vec3((n2.x / 255.f) * 2 - 1, (n2.y / 255.f) * 2 - 1, (n2.z / 255.f) * 2 -
  1); glm::vec3 N; N = glm::cross(vec3_sub(v1, v0), vec3_sub(v2, v0)); N =
  glm::normalize(N); if ((N0.x + N0.y + N0.z) == 0) { N0 = N;
      }
      else {
          N0 = vec3_add(N0, N);
          N0 = vec3_fdiv(N0, 2);
          N0 = glm::normalize(N0);
      }
      if ((N1.x + N1.y + N1.z) == 0) {
          N1 = N;
      }
      else {
          N1 = vec3_add(N1, N);
          N1 = vec3_fdiv(N1, 2);
          N1 = glm::normalize(N1);
      }
      if ((N2.x + N2.y + N2.z) == 0) {
          N2 = N;
      }
      else {
          N2 = vec3_add(N2, N);
          N2 = vec3_fdiv(N2, 2);
          N2 = glm::normalize(N2);
      }
      n0.x = ((N0.x + 1) * 0.5) * 255.f;
      n0.y = ((N0.y + 1) * 0.5) * 255.f;
      n0.z = ((N0.z + 1) * 0.5) * 255.f;
      n1.x = ((N1.x + 1) * 0.5) * 255.f;
      n1.y = ((N1.y + 1) * 0.5) * 255.f;
      n1.z = ((N1.z + 1) * 0.5) * 255.f;
      n2.x = ((N2.x + 1) * 0.5) * 255.f;
      n2.y = ((N2.y + 1) * 0.5) * 255.f;
      n2.z = ((N2.z + 1) * 0.5) * 255.f;
  }
  */
    auto mtl = Material::Create("default_terrain");
    // mtl->set_texture_albedo(texture);
    // mtl->set_texture_roughness(texture);
    mtl->albedo = glm::vec3(0.5);
    mtl->roughness = 0.5;
    vg->SetMaterial(mtl);
    terrain->Addvg);
    return terrain;
}

std::shared_ptr<Terrain> Terrain::Create(const std::string& name,
    glm::ivec2 resolution,
    const std::string& path)
{
    GDALAllRegister();
    auto data = (GDALDataset*)GDALOpen(path.c_str(), GA_ReadOnly);
    if (data == nullptr) {
        throw std::runtime_error(path + " : Could not open");
    }
    auto band = data->GetRasterBand(1);
    if (band == nullptr) {
        throw std::runtime_error(path + " : Could not Get raster band.");
    }
    double gt[6];
    auto err1 = data->GetGeoTransform(gt);
    glm::vec2 size { float(band->GetXSize()), float(band->GetYSize()) };
    float* scanLine = (float*)CPLMalloc(sizeof(float) * size.x * size.y);
    auto err = band->RasterIO(GF_Read, 0, 0, size.x, size.y, scanLine, size.x,
        size.y, GDT_Float32, 0, 0);
    if (err1 != CE_None || err != CE_None) {
        debugLog("Something went wrong with : " + path);
        return Terrain::Create(
            name, resolution, glm::vec3(1, 1, 1),
            TextureParser::parse(name + "_terrainData", path));
    }
    std::string proj = data->GetProjectionRef();
    debugLog(proj);
    GDALClose(data);
    auto t = Texture::Create(name + "_terrainData", size, GL_RED, GL_COMPRESSED_RED, GL_FLOAT, scanLine);
    CPLFree(scanLine);
    return Terrain::Create(name, resolution,
        glm::vec3(size.x * gt[1], 1, size.y * -gt[5]), t);
}
#endif