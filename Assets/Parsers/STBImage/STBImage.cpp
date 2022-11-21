#include <Assets/Asset.hpp>
#include <SG/Buffer/Buffer.hpp>
#include <SG/Buffer/View.hpp>
#include <SG/Image/Image.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <memory>
#include <fstream>

namespace TabGraph::Assets
{
std::shared_ptr<Asset> ParseSTBImage(const std::shared_ptr<Asset>& a_Container) {
	auto path = a_Container->GetUri().DecodePath();
	std::ifstream file(path, std::ios_base::binary);
	int width = 0, height = 0, comp = 0;
	stbi_io_callbacks cb{};
	cb.read = [](void* a_User, char* a_Data, int a_Size) -> int {
		((std::istream*)a_User)->read(a_Data, a_Size);
		return ((std::istream*)a_User)->gcount();
	};
	cb.skip = [](void* a_User, int a_N) {
		((std::istream*)a_User)->ignore(a_N);
	};
	cb.eof = [](void* a_User) -> int {
		return ((std::istream*)a_User)->peek() == EOF;
	};
	auto string = path.u8string();
	auto bytes = stbi_load(string.c_str(), &width, &height, &comp, 0);
	//auto bytes = stbi_load_from_callbacks(&cb, &file, &width, &height, &comp, 0);
	auto buffer = std::make_shared<SG::Buffer>(std::vector<std::byte>((std::byte*)bytes, (std::byte*)bytes + (width * height * comp)));
	stbi_image_free(bytes);
	auto image = std::make_shared<SG::Image>();
	switch (comp) {
	case 1:
		image->SetPixelDescription(SG::Pixel::SizedFormat::Uint8_NormalizedR);
		break;
	case 2:
		image->SetPixelDescription(SG::Pixel::SizedFormat::Uint8_NormalizedRG);
		break;
	case 3:
		image->SetPixelDescription(SG::Pixel::SizedFormat::Uint8_NormalizedRGB);
		break;
	case 4:
		image->SetPixelDescription(SG::Pixel::SizedFormat::Uint8_NormalizedRGBA);
		break;
	}
	image->SetBufferView(std::make_shared<SG::BufferView>(buffer, 0, buffer->size()));
	image->SetSize({ width, height, 1 });
	image->SetType(SG::Image::Type::Image2D);
	a_Container->assets.push_back(image);
	a_Container->SetLoaded(true);
	return a_Container;
}

}