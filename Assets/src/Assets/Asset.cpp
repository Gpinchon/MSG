#include <Assets/Asset.hpp>

namespace TabGraph::Assets {
Asset::Asset() {
	static auto s_assetNbr = 0u;
	SetName("Asset_" + std::to_string(++s_assetNbr));
};
}


