#include <Assets/Asset.hpp>

TabGraph::Assets::Asset::Asset(const Uri& uri) : _Uri(uri)
{
	static auto s_assetNbr = 0u;
	SetName("Asset_" + std::to_string(++s_assetNbr));
}
