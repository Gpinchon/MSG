/*
 * @Author: gpinchon
 * @Date:   2020-08-18 13:46:27
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-07-04 15:31:08
 */

#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>

#include <Tools/LazyConstructor.hpp>
#include <Tools/ThreadPool.hpp>

#include <algorithm>
#include <assert.h>
#include <filesystem>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

namespace TabGraph::Assets {
static Tools::ThreadPool s_ThreadPool;

auto& _getParsers()
{
    static std::unordered_map<Parser::MimeType, Parser> s_parsers;
    return s_parsers;
}

auto& _getMimesExtensions()
{
    static std::unordered_map<Parser::FileExtension, Parser::MimeType> s_mimesExtensions;
    return s_mimesExtensions;
}

std::future<std::shared_ptr<Assets::Asset>> Parser::AddParsingTask(const std::shared_ptr<Assets::Asset>& a_Asset)
{
    return s_ThreadPool.Enqueue([asset = a_Asset] {
        return Parser::Parse(asset);
    });
}

Parser::Parser(const MimeType& mimeType, ParsingFunction parsingFunction)
    : _mimeType(mimeType)
    , _parsingFunction(parsingFunction)
{
}

Parser::MimeType Parser::GetMimeFromExtension(const FileExtension& a_Extension)
{
    auto extension = a_Extension;
    std::transform(extension.begin(), extension.end(), extension.begin(), [](const auto& c) { return std::tolower(c); });
    return _getMimesExtensions()[FileExtension(extension)];
}

Parser& Parser::Add(const MimeType& mimeType, ParsingFunction parsingFunction)
{
    Tools::LazyConstructor lazyConstructor =
        [&mimeType, &parsingFunction]() {
            return Parser(mimeType, parsingFunction);
        };
    return _getParsers().try_emplace(mimeType, lazyConstructor).first->second;
}

Parser::MimeExtensionPair Parser::AddMimeExtension(const MimeType& a_Mime, const FileExtension& a_Extension)
{
    auto extension = a_Extension;
    std::transform(extension.begin(), extension.end(), extension.begin(), [](const auto& c) { return std::tolower(c); });
    _getMimesExtensions()[extension] = a_Mime;
    return MimeExtensionPair(a_Mime, extension);
}

std::shared_ptr<Assets::Asset> Parser::Parse(std::shared_ptr<Assets::Asset> a_Asset)
{
    assert(a_Asset != nullptr);
    auto uriScheme = a_Asset->GetUri().GetScheme();
    MimeType mime;
    if (uriScheme == "data")
        mime = DataUri(a_Asset->GetUri()).GetMime();
    else
        mime = GetMimeFromExtension(a_Asset->GetUri().DecodePath().extension().string());
    a_Asset->SetAssetType(mime);
    auto parser = _get(mime);
    if (parser != nullptr)
        return parser(a_Asset);
    return a_Asset;
}

Parser::ParsingFunction Parser::_get(const MimeType& mime)
{
    auto parser = _getParsers().find(mime);
    return parser != _getParsers().end() ? parser->second._parsingFunction : nullptr;
}
}
