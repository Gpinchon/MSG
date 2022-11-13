/*
* @Author: gpinchon
* @Date:   2020-08-18 13:46:27
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-04 15:31:08
*/

#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Debug.hpp>
#include <Events/DispatchQueue.hpp>
#include <Events/Manager.hpp>

#include <assert.h>
#include <filesystem>
#include <map>
#include <memory>
#include <set>
#include <string>

namespace TabGraph::Assets {
static std::mutex s_parsingTaskMutex;
static std::condition_variable s_cv;
static std::set<std::weak_ptr<Assets::Asset>, std::owner_less<>> s_parsingAssets;

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

inline void NotifyLoaded(std::shared_ptr<Assets::Asset> asset)
{
    Events::Event event;
    event.type = Events::Event::Type::AssetLoaded;
    event.data = Events::Event::Asset {
        asset->GetLoaded(),
        asset
    };
    Events::Manager::PushEvent(event);
    s_parsingTaskMutex.lock();
    s_parsingAssets.erase(asset);
    s_parsingTaskMutex.unlock();
    s_cv.notify_all();
}

void Parser::AddParsingTask(const ParsingTask& parsingTask)
{
    auto sharedAsset { parsingTask.asset.lock() };
    {
        std::unique_lock<std::mutex> lock(s_parsingTaskMutex);
        if (parsingTask.type == ParsingTask::Type::Sync) {
            s_cv.wait(lock, [parsingTask] {
                return s_parsingAssets.count(parsingTask.asset) == 0;
            });
        } else if (s_parsingAssets.count(sharedAsset) > 0)
            return;
        s_parsingAssets.insert(sharedAsset);
    }
    if (sharedAsset->GetLoaded())
        NotifyLoaded(sharedAsset);
    else if (parsingTask.type == ParsingTask::Type::Sync) {
        std::unique_lock<std::mutex> lock(sharedAsset->GetLock());
        Parser::Parse(sharedAsset);
        NotifyLoaded(sharedAsset);
    } else {
        auto weakAsset { parsingTask.asset };
        Events::DispatchQueue::ApplicationDispatchQueue().Dispatch([weakAsset] {
            auto sharedAsset { weakAsset.lock() };
            std::unique_lock<std::mutex> lock(sharedAsset->GetLock());
            Parser::Parse(sharedAsset);
            NotifyLoaded(sharedAsset);
        });
    }
}

Parser::Parser(const MimeType& mimeType, ParsingFunction parsingFunction)
    : _mimeType(mimeType)
    , _parsingFunction(parsingFunction)
{
}

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

bool Parser::Parse(std::shared_ptr<Assets::Asset> asset)
{
    assert(asset != nullptr);
    auto uriScheme = asset->GetUri().GetScheme();
    MimeType mime;
    if (uriScheme == "data")
        mime = DataUri(asset->GetUri()).GetMime();
    else
        mime = GetMimeFromExtension(Parser::FileExtension(asset->GetUri().DecodePath()).extension());
    auto parser = _get(mime);
    if (parser != nullptr) {
        parser(asset);
        return true;
    }
    return false;
}

Parser::ParsingFunction Parser::_get(const MimeType& mime)
{
    auto& parser = _getParsers()[mime];
    return parser ? parser->_parsingFunction : nullptr;
}
}