/*
* @Author: gpinchon
* @Date:   2020-08-18 13:46:27
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-04 15:31:08
*/

#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>

#include <Tools/ThreadPool.hpp>

#include <assert.h>
#include <filesystem>
#include <map>
#include <memory>
#include <set>
#include <string>

namespace TabGraph::Assets {
static Tools::ThreadPool<8> s_ThreadPool;

std::map<Parser::MimeType, std::unique_ptr<Parser>>& _getParsers()
{
    static std::map<Parser::MimeType, std::unique_ptr<Parser>> s_parsers;
    return s_parsers;
}

std::map<Parser::FileExtension, Parser::MimeType>& _getMimesExtensions()
{
    static std::map<Parser::FileExtension, Parser::MimeType> s_mimesExtensions;
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
{}

Parser::MimeType Parser::GetMimeFromExtension(const FileExtension& extension)
{
    return _getMimesExtensions()[extension];
}

Parser& Parser::Add(const MimeType& mimeType, ParsingFunction parsingFunction)
{
    auto parser { new Parser(mimeType, parsingFunction) };
    _getParsers()[mimeType].reset(parser);
    return *parser;
}

Parser::MimeExtensionPair Parser::AddMimeExtension(const MimeType& mime, const FileExtension& extension)
{
    _getMimesExtensions()[extension] = mime;
    return MimeExtensionPair(mime, extension);
}

std::shared_ptr<Assets::Asset> Parser::Parse(std::shared_ptr<Assets::Asset> a_Asset)
{
    assert(a_Asset != nullptr);
    auto uriScheme = a_Asset->GetUri().GetScheme();
    MimeType mime;
    if (uriScheme == "data")
        mime = DataUri(a_Asset->GetUri()).GetMime();
    else mime = GetMimeFromExtension(Parser::FileExtension(a_Asset->GetUri().DecodePath()).extension());
    auto parser = _get(mime);
    if (parser != nullptr)
        return parser(a_Asset);
    return a_Asset;
}

Parser::ParsingFunction Parser::_get(const MimeType& mime)
{
    auto& parser = _getParsers()[mime];
    return parser ? parser->_parsingFunction : nullptr;
}
}