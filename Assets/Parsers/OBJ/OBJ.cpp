#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>

#include <SG/Component/Mesh.hpp>
#include <SG/Core/Buffer/Accessor.hpp>
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Primitive.hpp>
#include <SG/Entity/Node.hpp>
#include <SG/Entity/NodeGroup.hpp>
#include <SG/Scene/Scene.hpp>

#include <Tools/Debug.hpp>
#include <Tools/Pi.hpp>
#include <Tools/Tools.hpp>

#include <filesystem>
#include <fstream>
#include <strstream>
#include <unordered_map>

#include <glm/common.hpp>

namespace TabGraph::Assets {
struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

struct VertexIndice {
    unsigned position;
    unsigned texCoord;
    unsigned normal;
};

struct Face : std::vector<VertexIndice> {
    Face()
    {
        reserve(3);
    }
    SG::Component::Name object;
    SG::Component::Name group;
    SG::Component::Name material;
};

struct OBJDictionnary {
    OBJDictionnary()
    {
        positions.reserve(4096);
        texCoords.reserve(4096);
        normals.reserve(4096);
        faces.reserve(4096);
    }
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<Face> faces;
    std::unordered_map<std::string, std::shared_ptr<SG::Material>> materials {
        { "default", std::make_shared<SG::Material>("default") }
    };
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
    const std::string& a_Object,
    const std::string& a_Group,
    const std::string& a_Material,
    const OBJDictionnary& a_Dictionnary)
{
    Face face;
    face.object   = a_Object;
    face.group    = a_Group;
    face.material = a_Material;
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
            vertice.emplace_back(
                positions[index],
                texCoords[index],
                normals[index]);
        }
    }
    return vertice;
}

static auto GenerateMeshes(const std::shared_ptr<Assets::Asset>& a_Container, const OBJDictionnary& a_Dictionnary)
{
    std::vector<SG::Component::Mesh> meshes;
    auto buffer                      = std::make_shared<SG::Buffer>();
    SG::Component::Mesh* currentMesh = nullptr;
    size_t currentBufferOffset       = 0;
    std::string currentObject;
    std::string currentGroup;
    std::string currentMaterial;
    for (auto& face : a_Dictionnary.faces) {
        if (face.object != currentObject) {
            currentObject = face.object;
            currentMesh   = &meshes.emplace_back(currentObject);
        }
        if (face.object != currentObject || face.group != currentGroup || face.material != currentMaterial) {
            currentGroup    = face.group;
            currentMaterial = face.material;
            if (buffer->size() != 0) {
                auto vertexCount = buffer->size() / sizeof(Vertex);
                auto bufferView  = std::make_shared<SG::BufferView>(buffer, currentBufferOffset, buffer->size(), sizeof(Vertex));
                auto primitive   = std::make_shared<SG::Primitive>(currentGroup + currentMaterial);
                primitive->SetPositions(SG::BufferAccessor(bufferView,
                    0, vertexCount,
                    SG::DataType::Float32, 3));
                primitive->SetTexCoord0(SG::BufferAccessor(bufferView,
                    sizeof(Vertex::position), vertexCount,
                    SG::DataType::Float32, 2));
                primitive->SetNormals(SG::BufferAccessor(bufferView,
                    sizeof(Vertex::position) + sizeof(Vertex::texCoord), vertexCount,
                    SG::DataType::Float32, 3));
                primitive->GenerateTangents();
                currentMesh->primitives[primitive] = a_Container->GetByName<SG::Material>(currentMaterial).front();
                currentBufferOffset                = buffer->size();
            }
        }
        for (auto& vertex : TriangulateFace(face, a_Dictionnary)) {
            buffer->push_back(vertex);
        }
    }
    if (buffer->size() != 0 && currentMesh != nullptr && currentMesh->primitives.empty()) {
        auto vertexCount = buffer->size() / sizeof(Vertex);
        auto bufferView  = std::make_shared<SG::BufferView>(buffer, currentBufferOffset, buffer->size(), sizeof(Vertex));
        auto primitive   = std::make_shared<SG::Primitive>(currentGroup + currentMaterial);
        primitive->SetPositions(SG::BufferAccessor(bufferView,
            0, vertexCount,
            SG::DataType::Float32, 3));
        primitive->SetTexCoord0(SG::BufferAccessor(bufferView,
            sizeof(Vertex::position), vertexCount,
            SG::DataType::Float32, 2));
        primitive->SetNormals(SG::BufferAccessor(bufferView,
            sizeof(Vertex::position) + sizeof(Vertex::texCoord), vertexCount,
            SG::DataType::Float32, 3));
        primitive->GenerateTangents();
        currentMesh->primitives[primitive] = a_Container->GetByName<SG::Material>(currentMaterial).front();
        currentBufferOffset                = buffer->size();
    }
    return meshes;
}

static void ParseMTLLIB(const Uri& a_Uri, const std::shared_ptr<Assets::Asset>& a_Container)
{
    auto asset            = std::make_shared<Assets::Asset>(a_Uri);
    asset->parsingOptions = a_Container->parsingOptions;
    a_Container->MergeObjects(Parser::Parse(asset));
}

static void StartOBJParsing(std::istream& a_Stream, const std::shared_ptr<Assets::Asset>& a_Container)
{
    OBJDictionnary dictionnary;
    std::string line;
    std::string currentObject   = "default";
    std::string currentGroup    = "default";
    std::string currentMaterial = "default";
    while (std::getline(a_Stream, line)) {
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
            dictionnary.faces.push_back(ParseFace(args, currentObject, currentGroup, currentMaterial, dictionnary));
        } else if (args.at(0) == "usemtl") {
            currentMaterial = args.at(1);
        } else if (args.at(0) == "g") {
            currentGroup = args.at(1);
        } else if (args.at(0) == "o") {
            currentObject = args.at(1);
        } else if (args.at(0) == "mtllib") {
            std::filesystem::path parentPath = a_Container->GetUri().DecodePath().parent_path();
            std::filesystem::path file;
            for (auto i = 1u; i < args.size(); i++)
                file += args.at(i);
            ParseMTLLIB(parentPath / file, a_Container);
        }
    }
    auto scene     = std::make_shared<SG::Scene>(a_Container->GetECSRegistry());
    auto& rootNode = scene->GetRootEntity();
    for (auto mesh : GenerateMeshes(a_Container, dictionnary)) {
        auto node = SG::Node::Create(a_Container->GetECSRegistry());
        node.AddComponent<SG::Component::Mesh>(mesh);
        SG::Node::SetParent(node, rootNode);
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
