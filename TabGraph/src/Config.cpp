/*
* @Author: gpinchon
* @Date:   2020-08-27 18:48:19
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:41:51
*/

#include <Config.hpp>
#include <Debug.hpp>

#include <exception>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdio.h> // for sscanf, fgets, fopen
#include <vector>

namespace TabGraph::Config {
void File::Parse(const std::filesystem::path path)
{
    std::ifstream configFile(path);
    for (std::string line; std::getline(configFile, line);) {
        std::istringstream iss(line);
        std::vector<std::string> words(
            std::istream_iterator<std::string> { iss },
            std::istream_iterator<std::string>());
        if (words.size() > 2 && words.at(1) == "=") {
            switch (words.size() - 2) {
            case 1:
                try {
                    auto value{ std::stof(words.at(2)) };
                    _configMap[words.at(0)] = value;
                } catch (std::exception&) {
                    _configMap[words.at(0)] = words.at(2);
                }
                break;
            case 2:
                _configMap[words.at(0)] = glm::vec2(std::stof(words.at(2)), std::stof(words.at(3)));
                break;
            case 3:
                _configMap[words.at(0)] = glm::vec3(std::stof(words.at(2)), std::stof(words.at(3)), std::stof(words.at(4)));
                break;
            }
        }
    }
}

void File::Save(const std::filesystem::path path)
{
    std::ofstream configFile;
    configFile.open(path);
    for (const auto& v : _configMap) {
        configFile << v.first << " = ";
        switch (v.second.index()) {
        case 0:
            configFile << std::get<0>(v.second);
            break;
        case 1:
            configFile << std::get<1>(v.second).x << ' ' << std::get<1>(v.second).y;
            break;
        case 2:
            configFile << std::get<2>(v.second).x << ' ' << std::get<2>(v.second).y << ' ' << std::get<2>(v.second).z;
            break;
        case 3:
            configFile << std::get<3>(v.second);
            break;
        }
        configFile << '\n';
    }
}

File& Config::Global()
{
    static Config::File instance;
    return instance;
}

}
