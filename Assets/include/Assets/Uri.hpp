/*
 * @Author: gpinchon
 * @Date:   2021-02-01 13:53:30
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-07-01 22:30:42
 */
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <array>
#include <filesystem>
#include <map>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Assets {
/**
 * @brief Parses Uri to make it more understandable to humans
 * This ignores spaces on purpose so make sure to convert them to %20
 * @ref https://www.ietf.org/rfc/rfc2396.txt
 */
class Uri {
public:
    Uri() = default;
    /**
     * @brief Parses uri string into Uri
     * @param uri a uri containing no space
     */
    Uri(const std::string& uri);
    /**
     * @brief Creates a URI from a file path
     */
    Uri(const std::filesystem::path& filePath);
    void SetScheme(const std::string& str);
    std::string GetScheme() const;
    void SetAuthority(const std::string& str);
    std::string GetAuthority() const;
    void SetPath(const std::string& str);
    std::string GetPath() const;
    std::filesystem::path DecodePath() const;
    static std::string Encode(const std::string& uri);
    static std::string Decode(const std::string& uri);
    void SetQuery(const std::string& str);
    std::string GetQuery() const;
    void SetFragment(const std::string& str);
    std::string GetFragment() const;
    operator std::string() const;
    friend std::ostream& operator<<(std::ostream& os, const Uri& uri);
    inline bool operator==(const Uri& other) const
    {
        return operator std::string() == other.operator std::string();
    }
    inline bool operator!=(const Uri& other) const
    {
        return !operator==(other);
    };

private:
    std::string _scheme { "" };
    std::string _authority { "" };
    std::string _path { "" };
    std::string _query { "" };
    std::string _fragment { "" };
};

/**
 * @brief Converts a Uri into a data uri.
 */
class DataUri {
public:
    /**
     * @brief Silently creates an empty data uri (data:,) if Uri is not a data scheme
     * @param uri a data Uri
     */
    DataUri(const Uri& uri = {});
    void SetMime(const std::string& mime);
    std::string GetMime() const;
    void SetParameter(const std::string& attribute, const std::string& value);
    std::string GetParameter(const std::string& attribute) const;
    std::map<std::string, std::string> GetParameters() const;
    void SetBase64(bool base64);
    bool GetBase64() const;
    void SetData(const std::string& a_Data);
    const std::string& GetData() const;
    operator std::string();
    /**
     * @brief Decodes the data uri to native std::byte
     * @return the converted data in the form of a vector of std::byte
     */
    std::vector<std::byte> Decode() const;

private:
    bool _base64 { false };
    std::string _mime {};
    std::map<std::string, std::string> _parameters {};
    std::string _data {};
};
}
