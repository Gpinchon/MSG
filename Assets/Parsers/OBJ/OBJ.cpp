/*
* @Author: gpinchon
* @Date:   2020-08-17 14:43:37
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-10 22:09:47
*/
/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-09 17:11:29
*/

#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>

#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/Accessor.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Primitive.hpp>

#include <SG/Component/Mesh.hpp>

#include <SG/Entity/Node.hpp>

#include <SG/Scene/Scene.hpp>

#include <Tools/Debug.hpp>
#include <Tools/Tools.hpp>
#include <Tools/Pi.hpp>

#include <sstream>
#include <memory>
#include <algorithm>
#include <errno.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <math.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <vector>

#ifdef _WIN32
#include <io.h>
#ifndef R_OK
#define R_OK 4
#endif
#else
#include <sys/io.h>
#endif // for access, R_OK

namespace TabGraph::Assets {
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

glm::vec3 generate_vn(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
    return (glm::normalize(glm::cross(v1 - v0, v2 - v0)));
}

glm::vec2 generate_vt(glm::vec3 v, glm::vec3 center) {
    glm::vec2 vt{ 0, 0 };
    glm::vec3 vec{ 0, 0, 0 };

    vec = glm::normalize(center - v);
    vt.x = 0.5f + (atan2(vec.z, vec.x) / (2 * M_PI));
    vt.y = 0.5f + -vec.y * 0.5f;
    return (vt);
}

void correct_vt(glm::vec2& vt0, glm::vec2& vt1, glm::vec2& vt2)
{
    glm::vec3 v[3]{};
    glm::vec3 texnormal{ 0, 0, 0 };

    v[0] = glm::vec3(vt0, 0.f);
    v[1] = glm::vec3(vt1, 0.f);
    v[2] = glm::vec3(vt2, 0.f);
    texnormal = glm::cross(v[1] - v[0], v[2] - v[0]);
    if (texnormal.z > 0) {
        if (vt0.x < 0.25f)
            vt0.x += 1.f;
        if (vt1.x < 0.25f)
            vt1.x += 1.f;
        if (vt2.x < 0.25f)
            vt2.x += 1.f;
    }
}

auto StrSplit(const std::string& a_String, const std::string::value_type& a_Delimiter) {
    std::stringstream ss(a_String);
    std::string segment;
    std::vector<std::string> seglist;

    while (std::getline(ss, segment, a_Delimiter)) {
        seglist.push_back(segment);
    }
    return seglist;
}

std::vector<std::string> StrSplitWSpace(const std::string& input)
{
    std::istringstream buffer(input);
    return {
        (std::istream_iterator<std::string>(buffer)),
        std::istream_iterator<std::string>() };
}

auto StrCountChar(const std::string& a_Input, const std::string::value_type& a_Character) {
    return size_t(std::count(a_Input.begin(), a_Input.end(), a_Character));
}

struct ObjContainer {
    std::shared_ptr<Assets::Asset>  container { std::make_shared<Assets::Asset>() };
    std::vector<SG::Component::Mesh> meshes;
    std::shared_ptr<SG::Primitive>   currentGeometry { nullptr };
    std::shared_ptr<SG::Buffer>     currentBuffer { nullptr };
    std::shared_ptr<SG::BufferView> currentBufferView{ nullptr };
    std::vector<glm::vec3> v;
    std::vector<glm::vec3> vn;
    std::vector<glm::vec2> vt;
    std::filesystem::path path;
};

static int get_vi(const std::vector<glm::vec2>& v, const std::string& str)
{
    int vindex;

    vindex = std::stoi(str);
    if (vindex < 0) {
        vindex = v.size() + vindex;
    } else {
        vindex -= 1;
    }
    if (vindex < 0 || static_cast<unsigned>(vindex) >= v.size()) {
        return (-1);
    }
    return (vindex);
}

static int get_vi(const std::vector<glm::vec3>& v, const std::string& str)
{
    int vindex;

    vindex = std::stoi(str);
    if (vindex < 0) {
        vindex = v.size() + vindex;
    } else {
        vindex -= 1;
    }
    if (vindex < 0 || static_cast<unsigned>(vindex) >= v.size()) {
        return (-1);
    }
    return (vindex);
}

static auto parse_indice(ObjContainer& p, const std::vector<std::string>& split)
{
    std::array<std::array<int, 3>, 3> vindex{};
    for (auto i = 0; i < 3; i++) {
        vindex.at(i).at(0) = -1;
        vindex.at(i).at(1) = -1;
        vindex.at(i).at(2) = -1;
    }
    for (auto i = 0u; i < split.size() && i < 3; i++) {
        auto fsplit = StrSplit(split.at(i), '/');
        auto splitLen = fsplit.size();
        auto slashCount = StrCountChar(split[i], '/');
        vindex.at(0).at(i) = -1;
        vindex.at(1).at(i) = -1;
        vindex.at(2).at(i) = -1;
        vindex.at(0).at(i) = get_vi(p.v, fsplit[0]);
        if (vindex.at(0).at(i) == -1) {
            return vindex;
        }
        if ((splitLen == 3 && slashCount == 2) || (splitLen == 2 && slashCount == 1)) {
            vindex.at(2).at(i) = get_vi(p.vt, fsplit[1]);
        }
        if (splitLen == 3 && slashCount == 2) {
            vindex.at(1).at(i) = get_vi(p.vn, fsplit[2]);
        } else if (splitLen == 2 && slashCount == 2) {
            vindex.at(1).at(i) = get_vi(p.vn, fsplit[1]);
        }
    }
    return vindex;
}

static void parse_vn(ObjContainer& p, std::array<std::array<int, 3>, 3> vindex, glm::vec3 v[3], glm::vec3 vn[3])
{
    short i;

    i = 0;
    while (i < 3) {
        if (vindex.at(1).at(i) != -1) {
            vn[i] = p.vn[vindex.at(1).at(i)];
        } else {
            vn[i] = generate_vn(v[i], v[i + 1], v[i + 2]);
        }
        i++;
    }
}

static void push_values(ObjContainer& p, glm::vec3* v, glm::vec3* vn, glm::vec2* vt)
{
    if (p.currentBuffer == nullptr) {
        p.currentBuffer = std::make_shared<SG::Buffer>();
    }
    if (p.currentBufferView == nullptr) {
        p.currentBufferView->SetBuffer(p.currentBuffer);
        p.currentBufferView->SetByteStride(sizeof(Vertex));
        p.currentBufferView->SetByteLength(p.currentBuffer->GetByteSize());
    }
    if (p.currentGeometry->GetPositions().empty()) {
        SG::BufferAccessor positionAccessor(p.currentBufferView,
            p.currentBufferView->GetByteLength(), 0,    //offset, size
            SG::BufferAccessor::ComponentType::Float32, 3 //vec3
        );
        p.currentGeometry->SetPositions(positionAccessor);
    }
    if (p.currentGeometry->GetNormals().empty()) {
        SG::BufferAccessor normalAccessor(p.currentBufferView,
            p.currentBufferView->GetByteLength() + sizeof(glm::vec3), 0,
            SG::BufferAccessor::ComponentType::Float32, 3
        );
        normalAccessor.SetNormalized(true);
        p.currentGeometry->SetNormals(normalAccessor);
    }
    if (p.currentGeometry->GetTexCoord0().empty()) {
        SG::BufferAccessor texcoordAccessor(p.currentBufferView,
            p.currentBufferView->GetByteLength() + sizeof(glm::vec3) + sizeof(glm::vec3), 0,
            SG::BufferAccessor::ComponentType::Float32, 2
        );
        p.currentGeometry->SetTexCoord0(texcoordAccessor);
    }
    for (auto index(0u); index < 3; ++index) {
        Vertex vertex{ v[index], vn[index], vt[index] };
        p.currentBuffer->push_back(vertex);
    }
    p.currentBufferView->SetByteLength(p.currentBuffer->GetByteSize());
    auto vertexCount { p.currentGeometry->GetPositions().GetSize() + 3 };
    p.currentGeometry->GetPositions().SetSize(vertexCount);
    p.currentGeometry->GetNormals().SetSize(vertexCount);
    p.currentGeometry->GetTexCoord0().SetSize(vertexCount);
}

void parse_v(ObjContainer& p, const std::vector<std::string>& split, std::vector<glm::vec2> in_vt)
{
    glm::vec3 v[3]{};
    glm::vec3 vn[3]{};
    glm::vec2 vt[3]{};
    short i;

    auto vindex { parse_indice(p, split) };
    i = 0;
    while (i < 3) {
        if (vindex[0][i] == -1) {
            return;
        }
        v[i] = p.v[vindex[0][i]];
        if (vindex[2][i] != -1) {
            vt[i] = p.vt[vindex[2][i]];
            //in_vt = (glm::vec2*)0x1;
        } else {
            vt[i] = in_vt.empty() ? generate_vt(v[i], glm::vec3()) : in_vt.at(i);
        }
        i++;
    }
    parse_vn(p, vindex, v, vn);
    if (in_vt.empty())
        correct_vt(vt[0], vt[1], vt[2]);
    push_values(p, v, vn, vt);
}

void parse_vg(ObjContainer& p, const std::string& name = "")
{
    static int childNbr = 0;
    childNbr++;

    if (name == "") {
        p.currentGeometry = std::make_shared<SG::Primitive>((std::string)p.meshes.at(0).name + "_Geometry_" + std::to_string(childNbr));
    } else {
        p.currentGeometry = std::make_shared<SG::Primitive>(name);
    }
    p.meshes.at(0).primitives[p.currentGeometry] = nullptr;
}

glm::vec3 parse_vec3(std::vector<std::string>& split)
{
    glm::vec3 v { 0 };
    for (uint8_t i = 0; i < 3 && (i + 1) < split.size(); ++i)
        v[i] = std::stof(split.at(i + 1));
    return v;
}

glm::vec2 parse_vec2(std::vector<std::string>& split)
{
    glm::vec2 v { 0 };
    for (uint8_t i = 0; i < 2 && (i + 1) < split.size(); ++i)
        v[i] = std::stof(split.at(i + 1));
    return v;
}

void parse_vtn(ObjContainer& p, std::vector<std::string>& split)
{
    glm::vec3 v { 0, 0, 0 };
    glm::vec2 vt { 0, 0 };

    if (split[0] == "v") {
        v = parse_vec3(split);
        p.v.push_back(v);
    } else if (split[0] == "vn") {
        v = parse_vec3(split);
        p.vn.push_back(v);
    } else if (split[0] == "vt") {
        vt = parse_vec2(split);
        vt.y = vt.y;
        p.vt.push_back(vt);
    }
}

static void parse_f(ObjContainer& p, std::vector<std::string>& split)
{
    short faces;

    split.erase(split.begin());
    faces = split.size() - 3 + 1;
    for (short i = 0; i < faces; ++i) {
        if (faces == 2 && i == 0) {
            parse_v(p,
                { split.at(0), split.at(i + 1), split.at(i + 2) },
                { glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1) });
        } else if (faces == 2 && i >= 1) {
            auto lesplit = std::vector<std::string>();
            parse_v(p,
                { split.at(0), split.at(i + 1), split.at(i + 2) },
                { glm::vec2(0, 0), glm::vec2(1, 1), glm::vec2(1, 0) });
        } else {
            parse_v(p,
                { split.at(0), split.at(i + 1), split.at(i + 2) }, {});
        }
    }
}

static void parse_line(ObjContainer& p, const char* line)
{
    auto split = StrSplitWSpace(line);
    if (split.empty() || split[0][0] == '#') {
        return;
    }
    if (split[0][0] == 'v') {
        parse_vtn(p, split);
    } else if (split[0][0] == 'f') {
        parse_f(p, split);
    } else if (split[0][0] == 'g' || split[0][0] == 'o') {
        auto mtl { p.meshes.at(0).primitives[p.currentGeometry] };
        parse_vg(p, split[1]);
        p.meshes.at(0).primitives[p.currentGeometry] = mtl;
    } else if (split[0] == "usemtl") {
        if (p.currentGeometry == nullptr || !p.currentGeometry->GetPositions().empty())
            parse_vg(p);
        p.meshes.at(0).primitives.insert({
            p.currentGeometry,
            p.container->GetByName<SG::Material>(split.at(1)).at(0)
        });
    } else if (split[0] == "mtllib") {
        auto mtllibAsset { std::make_shared<Assets::Asset>((p.path.parent_path() / split[1]).string()) };
        Assets::Parser::Parse(mtllibAsset);
        p.container->Merge(mtllibAsset);
    }
}

static void start_obj_parsing(ObjContainer& p, const std::string& path)
{
    char line[4096];

    if (access(path.c_str(), R_OK) != 0) {
        throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
    }
    auto fd(fopen(path.c_str(), "r"));
    if (fd == nullptr) {
        throw std::runtime_error(std::string("Can't open ") + path + " : " + strerror(errno));
    }
    p.meshes.push_back(SG::Component::Mesh(path));
    auto l = 1;
    while (fgets(line, 4096, fd) != nullptr) {
        parse_line(p, line);
        l++;
    }
    fclose(fd);
}

std::shared_ptr<Assets::Asset> ParseOBJ(const std::shared_ptr<Assets::Asset>& container)
{
    ObjContainer p;

    p.path = container->GetUri().DecodePath();
    start_obj_parsing(p, p.path.string());
    container->Merge(p.container);
    auto scene(std::make_shared<SG::Scene>(container->GetECSRegistry(), p.path.string()));
    for (const auto& mesh : p.meshes) {
        auto entity = SG::Node::Create(container->GetECSRegistry());
        entity.AddComponent<SG::Component::Mesh>(mesh);
        scene->AddEntity(entity);
    }
    container->Add(scene);
    container->SetLoaded(true);
    return container;
}
}
