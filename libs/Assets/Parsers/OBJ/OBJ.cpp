#include <MSG/Assets/Asset.hpp>
#include <MSG/Assets/Parser.hpp>
#include <MSG/Debug.hpp>
#include <MSG/Entity/Node.hpp>
#include <MSG/Entity/NodeGroup.hpp>
#include <MSG/Material.hpp>
#include <MSG/MaterialSet.hpp>
#include <MSG/Mesh.hpp>
#include <MSG/Mesh/Primitive.hpp>
#include <MSG/Scene.hpp>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <strstream>
#include <unordered_set>

#include <glm/common.hpp>

namespace Msg::Assets {
struct VertexIndice {
    unsigned position;
    unsigned texCoord;
    unsigned normal;
};

struct VertexGroup {
    unsigned object   = 0;
    unsigned group    = 0;
    unsigned material = 0;
    unsigned start    = 0;
    unsigned end      = 0;
};

struct Face : std::vector<VertexIndice> {
    Face()
    {
        reserve(3);
    }
    unsigned object   = 0;
    unsigned group    = 0;
    unsigned material = 0;
};

struct OBJDictionnary {
    OBJDictionnary()
    {
        objects.reserve(4096);
        groups.reserve(4096);
        materials.reserve(4096);
        positions.reserve(4096);
        texCoords.reserve(4096);
        normals.reserve(4096);
        faces.reserve(4096);
    }
    std::vector<std::string> objects { "default" };
    std::vector<std::string> groups { "default" };
    std::vector<std::string> materials { "default" };
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<Face> faces;
};

static auto StrSplit(const std::string& a_String, const std::string::value_type& a_Delimiter)
{
    std::stringstream ss(a_String);
    std::string segment;
    std::vector<std::string> seglist;

    while (std::getline(ss, segment, a_Delimiter)) {
        seglist.push_back(segment);
    }
    return seglist;
}

static std::vector<std::string> StrSplitWSpace(const std::string& input)
{
    std::istringstream buffer(input);
    return {
        (std::istream_iterator<std::string>(buffer)),
        std::istream_iterator<std::string>()
    };
}

static Face ParseFace(
    const std::vector<std::string>& a_Args,
    const OBJDictionnary& a_Dictionnary)
{
    Face face;
    face.object   = a_Dictionnary.objects.size() - 1;
    face.group    = a_Dictionnary.groups.size() - 1;
    face.material = a_Dictionnary.materials.size() - 1;
    for (auto i = 1u; i < a_Args.size(); i++) {
        auto faceArgs = StrSplit(a_Args.at(i), '/');
        int v         = 0;
        int vt        = 0;
        int vn        = 0;
        if (faceArgs.size() >= 1 && !faceArgs.at(0).empty())
            v = std::stoi(faceArgs.at(0));
        if (faceArgs.size() >= 2 && !faceArgs.at(1).empty())
            vt = std::stoi(faceArgs.at(1));
        if (faceArgs.size() >= 3 && !faceArgs.at(2).empty())
            vn = std::stoi(faceArgs.at(2));
        if (v < 0)
            v = a_Dictionnary.positions.size() - v;
        if (vt < 0)
            vt = a_Dictionnary.texCoords.size() - vt;
        if (vn < 0)
            vn = a_Dictionnary.normals.size() - vn;
        face.emplace_back(v, vt, vn);
    }
    return face;
}

static auto GenerateNormal(const std::array<glm::vec3, 3>& a_Positions)
{
    return glm::normalize(glm::cross(a_Positions[1] - a_Positions[0], a_Positions[2] - a_Positions[0]));
}

static auto TriangulateFace(const Face& a_Face, const OBJDictionnary& a_Dictionnary)
{
    std::vector<Vertex> vertice;
    auto triangleCount = a_Face.size() - 3 + 1;
    for (auto triangleIndex = 0u; triangleIndex < triangleCount; triangleIndex++) {
        std::array<VertexIndice, 3> vi {
            a_Face.at(0),
            a_Face.at(triangleIndex + 1),
            a_Face.at(triangleIndex + 2)
        };
        std::array<glm::vec3, 3> positions {
            vi[0].position > 0 ? a_Dictionnary.positions.at(vi[0].position - 1) : glm::vec3 {},
            vi[1].position > 0 ? a_Dictionnary.positions.at(vi[1].position - 1) : glm::vec3 {},
            vi[2].position > 0 ? a_Dictionnary.positions.at(vi[2].position - 1) : glm::vec3 {}
        };
        std::array<glm::vec2, 3> texCoords {
            vi[0].texCoord > 0 ? a_Dictionnary.texCoords.at(vi[0].texCoord - 1) : glm::vec2 {},
            vi[1].texCoord > 0 ? a_Dictionnary.texCoords.at(vi[1].texCoord - 1) : glm::vec2 {},
            vi[2].texCoord > 0 ? a_Dictionnary.texCoords.at(vi[2].texCoord - 1) : glm::vec2 {}
        };
        std::array<glm::vec3, 3> normals {
            vi[0].normal > 0 ? a_Dictionnary.normals.at(vi[0].normal - 1) : GenerateNormal(positions),
            vi[1].normal > 0 ? a_Dictionnary.normals.at(vi[1].normal - 1) : GenerateNormal(positions),
            vi[2].normal > 0 ? a_Dictionnary.normals.at(vi[2].normal - 1) : GenerateNormal(positions)
        };
        for (auto index = 0u; index < 3; index++) {
            vertice.emplace_back(Vertex {
                .position = positions[index],
                .normal   = normals[index],
                .texCoord = { glm::vec2(texCoords[index].x, 1 - texCoords[index].y) },
            });
        }
    }
    return vertice;
}

static std::vector<std::pair<Mesh, MaterialSet>> GenerateMeshes(const std::shared_ptr<Assets::Asset>& a_Container, const OBJDictionnary& a_Dictionnary)
{
    std::vector<std::pair<Mesh, MaterialSet>> meshes;
    std::vector<VertexGroup> vertexGroups;

    std::vector<Vertex> vertice;
    size_t currentOffset = 0;
    for (auto& face : a_Dictionnary.faces) {
        if (vertexGroups.empty()
            || (face.object != vertexGroups.back().object || face.group != vertexGroups.back().group || face.material != vertexGroups.back().material)) {
            vertexGroups.emplace_back();
            vertexGroups.back().object   = face.object;
            vertexGroups.back().group    = face.group;
            vertexGroups.back().material = face.material;
            vertexGroups.back().start    = vertice.size();
        }
        for (auto& vertex : TriangulateFace(face, a_Dictionnary)) {
            vertice.push_back(vertex);
        }
        vertexGroups.back().end = vertice.size();
    }

    unsigned lastMtl    = -1u;
    unsigned mtlIndex   = -1u;
    unsigned lastObject = 0;
    for (auto& vg : vertexGroups) {
        auto& groupName    = a_Dictionnary.groups.at(vg.group);
        auto& materialName = a_Dictionnary.materials.at(vg.material);
        if (meshes.empty() || vg.object != lastObject) {
            mtlIndex = -1u;
            lastMtl  = -1u;
            meshes.emplace_back(Mesh(1), MaterialSet());
            lastObject = vg.object;
        }
        auto& mesh     = meshes.back();
        auto primitive = std::make_shared<MeshPrimitive>();
        primitive->SetVertices({ vertice.begin() + vg.start, vertice.begin() + vg.end });
        primitive->GenerateTangents();
        primitive->ComputeBoundingVolume();
        if (lastMtl != vg.material) {
            mtlIndex++;
            mesh.second.materials[mtlIndex] = a_Container->GetByName<Material>(materialName).front();
            lastMtl                         = vg.material;
        }
        mesh.first[0][primitive] = mtlIndex;
    }
    for (auto& mesh : meshes)
        mesh.first.ComputeBoundingVolume();
    return meshes;
}

static void ParseMTLLIB(const Uri& a_Uri, const std::shared_ptr<Assets::Asset>& a_Container)
{
    auto asset            = std::make_shared<Assets::Asset>(a_Uri);
    asset->parsingOptions = a_Container->parsingOptions;
    a_Container->MergeObjects(Parser::Parse(asset));
}

static std::filesystem::path GetFilePath(const std::string& a_Arg0, const std::string& a_Line, const std::filesystem::path& a_ParentPath)
{
    std::string file = a_Line;
    file.erase(file.find(a_Arg0), a_Arg0.size());
    file.erase(file.begin(), std::find_if(file.begin(), file.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return a_ParentPath / file;
}

static void StartOBJParsing(std::istream& a_Stream, const std::shared_ptr<Assets::Asset>& a_Container)
{
    OBJDictionnary dictionnary;
    std::unordered_set<std::filesystem::path> loadedMtlLibs;
    std::string line;
    auto parentPath = a_Container->GetUri().DecodePath().parent_path();
    while (std::getline(a_Stream, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        if (line.empty())
            continue;
        auto args = StrSplitWSpace(line);
        if (args.at(0) == "v") {
            dictionnary.positions.emplace_back(
                std::stof(args.at(1)),
                std::stof(args.at(2)),
                std::stof(args.at(3)));
        } else if (args.at(0) == "vn") {
            dictionnary.normals.emplace_back(
                std::stof(args.at(1)),
                std::stof(args.at(2)),
                std::stof(args.at(3)));
        } else if (args.at(0) == "vt") {
            dictionnary.texCoords.emplace_back(
                std::stof(args.at(1)),
                args.size() >= 3 ? std::stof(args.at(2)) : 0.f);
        } else if (args.at(0) == "f") {
            dictionnary.faces.push_back(ParseFace(args, dictionnary));
        } else if (args.at(0) == "usemtl") {
            dictionnary.materials.push_back(args.at(1));
        } else if (args.at(0) == "g") {
            dictionnary.groups.push_back(args.at(1));
        } else if (args.at(0) == "o") {
            dictionnary.objects.push_back(args.at(1));
        } else if (args.at(0) == "mtllib") {
            auto path = GetFilePath(args.at(0), line, parentPath);
            if (loadedMtlLibs.insert(path).second) // we did not load this mtllib yet
                ParseMTLLIB(path, a_Container);
        }
    }
    auto scene     = std::make_shared<Scene>(a_Container->GetECSRegistry());
    auto& rootNode = scene->GetRootEntity();
    for (auto mesh : GenerateMeshes(a_Container, dictionnary)) {
        auto node = Entity::Node::Create(a_Container->GetECSRegistry());
        node.AddComponent<Mesh>(mesh.first);
        node.AddComponent<MaterialSet>(mesh.second);
        Entity::Node::SetParent(node, rootNode);
    }
    a_Container->AddObject(scene);
}

std::shared_ptr<Assets::Asset> ParseOBJ(const std::shared_ptr<Assets::Asset>& a_Container)
{
    if (a_Container->GetUri().GetScheme() == "file") {
        auto stream = std::ifstream(a_Container->GetUri().DecodePath(), std::ios_base::in);
        StartOBJParsing(stream, a_Container);
    } else if (a_Container->GetUri().GetScheme() == "data") {
        auto binary = DataUri(a_Container->GetUri()).Decode();
        auto stream = std::istrstream(reinterpret_cast<const char*>(binary.data()), binary.size());
        StartOBJParsing(stream, a_Container);
    }
    a_Container->SetLoaded(true);
    return a_Container;
}
}
