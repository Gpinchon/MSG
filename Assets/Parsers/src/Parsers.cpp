
#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>

#include <memory>

namespace TabGraph::Assets {
//Forward declare parsing functions
std::shared_ptr<Asset> ParseBinaryData(const std::shared_ptr<Asset>&);
std::shared_ptr<Asset> ParseBMP(const std::shared_ptr<Asset>&);
std::shared_ptr<Asset> ParseFBX(const std::shared_ptr<Asset>&);
std::shared_ptr<Asset> ParseGLTF(const std::shared_ptr<Asset>&);

void InitParsers() {
    Parser::AddMimeExtension("application/octet-stream", ".bin");
    Parser::AddMimeExtension("model/gltf+json", ".gltf");
    Parser::Add("application/octet-stream", ParseBinaryData);
    Parser::Add("model/gltf+json", ParseGLTF);
}
}
