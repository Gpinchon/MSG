#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <filesystem>
#include <functional>
#include <future>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Assets {
class Asset;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Assets {
class Parser {
public:
    using FileExtension     = std::string;
    using MimeType          = std::string;
    using ParsingFunction   = std::function<std::shared_ptr<Assets::Asset>(const std::shared_ptr<Assets::Asset>&)>;
    using MimeExtensionPair = std::pair<MimeType, FileExtension>;
    using ParsingFuture     = std::future<std::shared_ptr<Assets::Asset>>;
    static ParsingFuture AddParsingTask(const std::shared_ptr<Assets::Asset>& a_Asset);
    /**
     * @brief Returns the MIME type if managed
     * @param extension the file extension to get the MIME type for
     * @return the corresponding MIME type or an empty string if not managed
     */
    static MimeType GetMimeFromExtension(const FileExtension& extension);
    /**
     * @brief Creates a new asset parser and returns a pointer to it, don't free it
     * @param mime the mime type this parser is for @ref https://en.wikipedia.org/wiki/Media_type
     * @param function the parsing function that will be used to parse that mime type
     * @return a pointer to the newly created Parser
     */
    static Parser& Add(const MimeType& mime, ParsingFunction function);
    /**
     * @brief Adds a new extension for this mime type
     * @param mime @ref https://en.wikipedia.org/wiki/Media_type
     * @param extension might be .jpeg, .jpg...
     * @return a pair of the mime type and the extension
     */
    static MimeExtensionPair AddMimeExtension(const MimeType& mime, const FileExtension& extension);
    /**
     * @brief Parses the specified asset using available parsers, won't do anything if no parser was found
     * @param asset the Asset to load, mime type is figured out using Asset's Uri
     */
    static std::shared_ptr<Assets::Asset> Parse(std::shared_ptr<Assets::Asset> asset);

private:
    Parser() = delete;
    Parser(const MimeType&, ParsingFunction);
    static ParsingFunction _get(const MimeType&);
    MimeType _mimeType;
    ParsingFunction _parsingFunction;
};
}
