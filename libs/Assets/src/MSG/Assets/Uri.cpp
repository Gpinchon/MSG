#include <MSG/Assets/Uri.hpp>
#include <MSG/Tools/Base.hpp>

#include <array> // for array
#include <charconv>
#include <codecvt>
#include <ctype.h> // for isalnum
#include <map> // for map, map<>::mapped_type
#include <regex>
#include <sstream> // for basic_istream
#include <string> // for getline

namespace MSG::Assets {
Uri::Uri(const std::string& rawUri)
{
    if (rawUri.length() == 0)
        return;
    auto uri = Encode(rawUri);
    /*
     * ^\s*                         <---- 0 Start by trimming leading white spaces
     *      (                       <---- 1 Optional scheme
     *          ([^:/?#]+):         <---- 2 "Clean" scheme (everything until we reach a #, a ?) followed by a :
     *      )?
     *      (\/\/                   <---- 3 Optional authority
     *          ([^/?#\s]*)         <---- 4 "Clean" authority (everything until we reach a #, a ? or a whitespace)
     *      )?
     *      ([^?#\s]*)              <---- 5 Path (everything until we reach a #, a ? or a whitespace)
     *      (\?                     <---- 6 Optional query
     *          ([^#\s]*)           <---- 7 "Clean" query (everything until we reach a # or a whitespace)
     *      )?
     *      (#                      <---- 8 Optional fragment
     *          (\S*)               <---- 9 "Clean" fragment (everything except white spaces)
     *      )?
     */
    // std::regex uriRegex { R"(^\s*(([^:/?#]+):)?(\/\/([^/?#\s]*))?([^?#\s]*)(\?([^#\s]*))?(#(\S*))?)", std::regex::ECMAScript };
    const auto searchEnd = std::sregex_iterator();
    ptrdiff_t offset     = 0u;
    {
        std::regex schemeRegex { R"(^\s*(?:([^:/?#]+):))", std::regex::ECMAScript };
        auto schemeResult = std::sregex_iterator(uri.begin(), uri.end(), schemeRegex);
        if (schemeResult != searchEnd) {
            SetScheme((*schemeResult)[1]);
            offset += (*schemeResult)[0].length();
        }
    }
    {
        std::regex authorityRegex { R"(^(?:\/\/([^/?#\s]*)))", std::regex::ECMAScript };
        auto authorityResult = std::sregex_iterator(uri.begin() + offset, uri.end(), authorityRegex);
        if (authorityResult != searchEnd) {
            SetAuthority((*authorityResult)[1]);
            offset += (*authorityResult)[0].length();
        }
    }
    {
        // this is too complex for poor lil' std regex :(
        auto pathEnd { uri.find_first_of("?# \t\n", offset) };
        SetPath(uri.substr(offset, pathEnd));
        if (pathEnd == std::string::npos)
            return;
        offset += pathEnd - offset;
    }
    {
        std::regex queryRegex { R"(^(?:\?([^#\s]*)))", std::regex::ECMAScript };
        auto queryResult { std::sregex_iterator(uri.begin() + offset, uri.end(), queryRegex) };
        if (queryResult != searchEnd) {
            SetPath(std::string((*queryResult)[1]));
            offset += (*queryResult)[0].length();
        }
    }
    {
        std::regex fragmentRegex { R"(^(?:\#([^#\s]*)))", std::regex::ECMAScript };
        auto fragmentResult { std::sregex_iterator(uri.begin() + offset, uri.end(), fragmentRegex) };
        if (fragmentResult != searchEnd) {
            SetPath(std::string((*fragmentResult)[1]));
            offset += (*fragmentResult)[0].length();
        }
    }
}

Uri::Uri(const std::filesystem::path& filePath)
{
    SetScheme("file");
    auto convertedPath = Encode(filePath.string());
    SetPath(convertedPath);
}

void Uri::SetScheme(const std::string& a_Str)
{
    _scheme = a_Str;
}

std::string Uri::GetScheme() const
{
    return _scheme;
}

void Uri::SetAuthority(const std::string& str)
{
    _authority = str;
}

std::string Uri::GetAuthority() const
{
    return _authority;
}

void Uri::SetPath(const std::string& str)
{
    _path = str;
}

std::string Uri::GetPath() const
{
    return _path;
}

std::filesystem::path Uri::DecodePath() const
{
    return Decode(GetPath());
}

/**
 * see rfc3986 section-2.3
 */
static inline bool IsUnreservedChar(const char& a_C)
{
    return std::isalnum(a_C)
        || a_C == '-'
        || a_C == '.'
        || a_C == '_'
        || a_C == '~';
}

/**
 * see rfc3986 section-2.2
 */
static constexpr bool IsReservedChar(const char& a_C)
{
    return a_C == '!'
        || a_C == '#'
        || a_C == '$'
        || a_C == '&'
        || a_C == '\''
        || a_C == '('
        || a_C == ')'
        || a_C == '*'
        || a_C == '+'
        || a_C == ','
        || a_C == '/'
        || a_C == ':'
        || a_C == ';'
        || a_C == '='
        || a_C == '?'
        || a_C == '@'
        || a_C == '['
        || a_C == ']';
}

std::string Uri::Encode(const std::string& value)
{
    static constexpr auto hex_chars = "0123456789"
                                      "ABCDEF";
    std::array<char, 3> rc          = { '%', 0, 0 };
    std::string encoded             = {};
    encoded.reserve(value.size());
    for (const auto& c : value) {
        if (IsReservedChar(c) || IsUnreservedChar(c))
            encoded.push_back(c);
        else {
            rc[1] = hex_chars[(c & 0xF0) >> 4];
            rc[2] = hex_chars[(c & 0x0F) >> 0];
            encoded.insert(encoded.end(), rc.begin(), rc.end());
        }
    }
    return encoded;
}

/**
 * see rfc5234 appendix-B.1
 */
static inline constexpr bool IsHexDig(const char& a_Char)
{
    return a_Char >= '0' && a_Char <= '9' || a_Char >= 'A' && a_Char <= 'F';
}

std::string Uri::Decode(const std::string& a_Encoded)
{
    if (a_Encoded.size() < 2)
        return "";
    char rc              = 0;
    std::string decoded  = a_Encoded;
    size_t dynamicLength = decoded.size() - 2;

    if (decoded.size() < 3)
        return decoded;
    for (size_t i = 0; i < dynamicLength; i++) {
        if (decoded[i] != '%'
            || !IsHexDig(decoded[i + 1])
            || !IsHexDig(decoded[i + 2]))
            continue;
        std::from_chars(&decoded[i + 1], &decoded[i + 3], rc, 16);
        decoded       = decoded.replace(i, 3, &rc, 1);
        dynamicLength = decoded.size() - 2;
    }
    return decoded;
}

void Uri::SetQuery(const std::string& str)
{
    _query = str;
}

std::string Uri::GetQuery() const
{
    return _query;
}

void Uri::SetFragment(const std::string& str)
{
    _fragment = str;
}

std::string Uri::GetFragment() const
{
    return _fragment;
}

Uri::operator std::string() const
{
    std::string fullUri;
    if (!GetScheme().empty())
        fullUri += GetScheme() + ":";
    if (!GetAuthority().empty())
        fullUri += "//" + GetAuthority();
    fullUri += GetPath();
    if (!GetQuery().empty())
        fullUri += "?" + GetQuery();
    if (!GetFragment().empty())
        fullUri += "#" + GetFragment();
    return fullUri;
}

DataUri::DataUri(const Uri& uri)
{
    /*
     * ^                        <---- 0 No need to trim, Uri already did it
     *  ([-\w]+/[-+\w.]+)?      <---- 1 Mime type
     *  (
     *      (?:;?[\w]+=[-\w]+)* <---- 2 Optional parameters, packed in one string
     *  )
     *  (;base64)?              <---- 3 Optional base64 value
     *  ,(.*)                   <---- 4 The data, we can just take everything there, should be clean
     * $
     */
    // std::regex dataRegex { R"(^([-+\w]+/[-+\w.]+)??((?:;?[\w]+=[-\w]+)*)(;base64)??,(.*)$)", std::regex::ECMAScript };
    const auto searchEnd = std::sregex_iterator();
    const auto& data { uri.GetPath() };
    size_t offset = 0u;
    {
        std::regex mimeRegex { R"(^([-+\w]+/[-+\w.]+))", std::regex::ECMAScript };
        auto mimeResult = std::sregex_iterator(data.begin(), data.end(), mimeRegex);
        if (mimeResult != searchEnd) {
            SetMime((*mimeResult)[1]);
            offset += (*mimeResult)[0].length();
        }
    }
    {
        std::regex parametersRegex { R"(^((?:;?[\w]+=[-\w]+)*))", std::regex::ECMAScript };
        auto parametersResult = std::sregex_iterator(data.begin() + offset, data.end(), parametersRegex);
        if (parametersResult != searchEnd) {
            std::stringstream parameters { (*parametersResult)[1] };
            std::string parameter;
            while (std::getline(parameters, parameter, ';')) {
                if (parameter.empty())
                    continue;
                auto separator { parameter.find(L'=') };
                auto attribute { parameter.substr(0, separator) };
                auto value { parameter.substr(separator + 1u) };
                SetParameter(attribute, value);
            }
            offset += (*parametersResult)[0].length();
        }
    }
    {
        std::regex base64Regex { R"(^(;base64))", std::regex::ECMAScript };
        auto base64Result = std::sregex_iterator(data.begin() + offset, data.end(), base64Regex);
        if (base64Result != searchEnd) {
            SetBase64((*base64Result)[1] == ";base64");
            offset += (*base64Result)[0].length();
        }
    }
    {
        // again, too complex for whiny std regex
        if ((data.begin() + offset) != data.end() && (data.begin() + offset)[0] == ',') {
            SetData(std::string(data.begin() + offset + 1, data.end())); //+1 to skip the comma
        }
    }
}

void DataUri::SetMime(const std::string& a_Mime)
{
    _mime = a_Mime;
}

std::string DataUri::GetMime() const
{
    return _mime;
}

void DataUri::SetParameter(const std::string& attribute, const std::string& value)
{
    _parameters[attribute] = value;
}

std::string DataUri::GetParameter(const std::string& attribute) const
{
    auto parameter = _parameters.find(attribute);
    if (parameter != _parameters.end())
        return parameter->second;
    return "";
}

std::map<std::string, std::string> DataUri::GetParameters() const
{
    return _parameters;
}

void DataUri::SetBase64(bool base64)
{
    _base64 = base64;
}

bool DataUri::GetBase64() const
{
    return _base64;
}

void DataUri::SetData(const std::string& a_Data)
{
    _data = a_Data;
}

const std::string& DataUri::GetData() const
{
    return _data;
}

DataUri::operator std::string()
{
    std::string fullUri = "data:";
    if (!GetMime().empty())
        fullUri += GetMime();
    if (!GetParameters().empty())
        for (const auto& parameter : GetParameters())
            fullUri += ';' + parameter.first + '=' + parameter.second;
    if (GetBase64())
        fullUri += ";base64";
    fullUri += ',' + GetData();
    return fullUri;
}

std::vector<std::byte> DataUri::Decode() const
{
    if (GetBase64())
        return Tools::Base64::Decode(GetData());
    else
        return Tools::Base32::Decode(GetData());
}

std::ostream& operator<<(std::ostream& os, const Uri& uri)
{
    os << std::string(uri);
    return os;
}
};
