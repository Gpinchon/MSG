/*
* @Author: gpinchon
* @Date:   2021-02-16 18:24:58
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-17 22:30:32
*/
#include <Tools/Base.hpp>
#include <cmath>
#include <cassert>

using namespace TabGraph::Tools;

std::string Base64::Encode(const std::vector<std::byte>& data)
{
    static constexpr auto B64table =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "+/";
    //full string is formed of 6 bits blocks
    const auto length = (data.size() + 2) / 3 * 4;
    std::string ret(length, '\0');
    auto pos { ret.begin() };
    auto in = data.begin();
    while (data.end() - in >= 3) {
        const auto inPtr { reinterpret_cast<const uint8_t*>(&in.operator*()) };
        *pos++ = B64table[((inPtr[0] >> 2))];
        *pos++ = B64table[((inPtr[0] & 0x03) << 4) | (inPtr[1] >> 4)];
        *pos++ = B64table[((inPtr[1] & 0x0f) << 2) | (inPtr[2] >> 6)];
        *pos++ = B64table[((inPtr[2] & 0x3f))];
        in += 3;
    }
    auto trailingBytes { data.end() - in };
    if (trailingBytes > 0) {
        const auto inPtr { reinterpret_cast<const uint8_t*>(&in.operator*()) };
        *pos++ = B64table[inPtr[0] >> 2];
        if (trailingBytes == 1) {
            *pos++ = B64table[((inPtr[0] & 0x03) << 4)];
            *pos++ = '=';
        } else {
            *pos++ = B64table[((inPtr[0] & 0x03) << 4) | (inPtr[1] >> 4)];
            *pos++ = B64table[((inPtr[1] & 0x0f) << 2)];
        }
        *pos++ = '=';
    }
    return ret;
}

static constexpr int B64index[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6,
    7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,
    0, 0, 0, 63, 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

std::vector<std::byte> Base64::Decode(const std::string& data)
{
    const auto len = data.length();
    const unsigned char* p = (unsigned char*)data.data();
    const int pad = len > 0 && (len % 4 || p[len - 1] == '=');
    const size_t L = ((len + 3) / 4 - pad) * 4;
    std::vector<std::byte> ret(L / 4 * 3 + pad);
    for (size_t i = 0, j = 0; i < L; i += 4) {
        int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
        ret[j++] = std::byte(n >> 16);
        ret[j++] = std::byte(n >> 8 & 0xFF);
        ret[j++] = std::byte(n & 0xFF);
    }
    if (pad) {
        int n = B64index[p[L]] << 18 | B64index[p[L + 1]] << 12;
        ret[ret.size() - 1] = std::byte(n >> 16);

        if (len > L + 2 && p[L + 2] != '=') {
            n |= B64index[p[L + 2]] << 6;
            ret.push_back(std::byte(n >> 8 & 0xFF));
        }
    }
    return ret;
}

/*
 * The encoded base32 string length is computed as follow :
 * ceil(bytesLength / 5.f) * 8
 * bytesLength / 5.f because we want to know how many chunks of 5 bytes we have, and ceil because 0.1 chunk is still 1 chunk
 * ceil(bytesLength / 5.f) * 8 because a chunk is made of 8 characters.
 * For the input data 'a' the encoded result will be ME====== because we have 1 chunk of 8 characters : two 5bits encoded characters(ME) 6 padding characters(======)
 * @ref https://tools.ietf.org/html/rfc4648#section-9
 */
std::string Base32::Encode(const std::vector<std::byte>& data)
{
    static constexpr auto B32Table =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "234567";
    std::string ret(std::ceil(data.size() / 5.0) * 8, '\0');
    auto pos { ret.begin() };
    auto in { data.begin() };
    //a 40bit input group is formed by concatenating 5 8bit input groups
    while (data.end() - in >= 5) {
        //these 40 bits are then treated as 8 concatenated 5-bit groups
        //(0xf8 >> 3)               == (11111000 >> 3)                   == 00011111
        //(0x07 << 2) | (0xc0 >> 6) == (00000111 << 2) | (11000000 >> 6) == 00011111
        //(0x3e >> 1)               == (00111110 >> 1)                   == 00011111
        //(0x01 << 4) | (0xf0 >> 4) == (00000001 << 4) | (11110000 >> 4) == 00011111
        //(0x0f << 1) | (0x80 >> 7) == (00001111 << 1) | (00000001 >> 7) == 00011111
        //(0x7c >> 2)               == (01111100 >> 2)                   == 00011111
        //(0x03 << 3) | (0xe0 >> 5) == (00000011 << 3) | (11100000 >> 5) == 00011111
        //(0x1f)                                                         == 00011111
        //let's get to bashing bits, as well as these bytes
        const auto inPtr { reinterpret_cast<const uint8_t*>(&in.operator*()) };
        *pos++ = B32Table[((inPtr[0] & 0xf8) >> 3)];
        *pos++ = B32Table[((inPtr[0] & 0x07) << 2) | ((inPtr[1] & 0xc0) >> 6)];
        *pos++ = B32Table[((inPtr[1] & 0x3e) >> 1)];
        *pos++ = B32Table[((inPtr[1] & 0x01) << 4) | ((inPtr[2] & 0xf0) >> 4)];
        *pos++ = B32Table[((inPtr[2] & 0x0f) << 1) | ((inPtr[3] & 0x80) >> 7)];
        *pos++ = B32Table[((inPtr[3] & 0x7c) >> 2)];
        *pos++ = B32Table[((inPtr[3] & 0x03) << 3) | ((inPtr[4] & 0xe0) >> 5)];
        *pos++ = B32Table[((inPtr[4] & 0x1f))];
        in += 5;
    }
    auto trailingBytes { data.end() - in };
    if (trailingBytes > 0) //check for trailing bytes
    {
        const auto inPtr { reinterpret_cast<const uint8_t*>(&in.operator*()) };
        *pos++ = B32Table[((inPtr[0] & 0xf8) >> 3)];
        if (trailingBytes == 1) {
            *pos++ = B32Table[((inPtr[0] & 0x07) << 2)];
            *pos++ = '=';
            *pos++ = '=';
            *pos++ = '=';
            *pos++ = '=';
            *pos++ = '=';
        } else if (trailingBytes == 2) {
            *pos++ = B32Table[((inPtr[0] & 0x07) << 2) | ((inPtr[1] & 0xc0) >> 6)];
            *pos++ = B32Table[((inPtr[1] & 0x3e) >> 1)];
            *pos++ = B32Table[((inPtr[1] & 0x01) << 4)];
            *pos++ = '=';
            *pos++ = '=';
            *pos++ = '=';
        } else if (trailingBytes == 3) {
            *pos++ = B32Table[((inPtr[0] & 0x07) << 2) | ((inPtr[1] & 0xc0) >> 6)];
            *pos++ = B32Table[((inPtr[1] & 0x3e) >> 1)];
            *pos++ = B32Table[((inPtr[1] & 0x01) << 4) | ((inPtr[2] & 0xf0) >> 4)];
            *pos++ = B32Table[((inPtr[2] & 0x0f) << 1)];
            *pos++ = '=';
            *pos++ = '=';
        } else if (trailingBytes == 4) {
            *pos++ = B32Table[((inPtr[0] & 0x07) << 2) | ((inPtr[1] & 0xc0) >> 6)];
            *pos++ = B32Table[((inPtr[1] & 0x3e) >> 1)];
            *pos++ = B32Table[((inPtr[1] & 0x01) << 4) | ((inPtr[2] & 0xf0) >> 4)];
            *pos++ = B32Table[((inPtr[2] & 0x0f) << 1) | ((inPtr[3] & 0x80) >> 7)];
            *pos++ = B32Table[((inPtr[3] & 0x7c) >> 2)];
            *pos++ = B32Table[((inPtr[3] & 0x03) << 3)];
        }
        *pos++ = '=';
    }
    return ret;
}

static inline uint8_t GetBase32Value(uint8_t value) {
    static const std::string table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    auto ret{ table.find(value) };
    return ret == std::string::npos ? 0 : ret;
}

std::vector<std::byte> Base32::Decode(const std::string& data)
{
    std::vector<std::byte> ret(data.size() / 1.6); //alternatively (data.size() * 5 / 8)
    auto pos{ ret.begin() };
    auto in{ data.begin() };
    while (data.end() - in >= 8) {
        const auto inPtr{ reinterpret_cast<const uint8_t*>(&in.operator*()) };
        uint8_t val0{ GetBase32Value(inPtr[0]) };
        uint8_t val1{ GetBase32Value(inPtr[1]) };
        uint8_t val2{ GetBase32Value(inPtr[2]) };
        uint8_t val3{ GetBase32Value(inPtr[3]) };
        uint8_t val4{ GetBase32Value(inPtr[4]) };
        uint8_t val5{ GetBase32Value(inPtr[5]) };
        uint8_t val6{ GetBase32Value(inPtr[6]) };
        uint8_t val7{ GetBase32Value(inPtr[7]) };
        //(0x1f << 3) | (0x1c >> 2)               == 00011111 << 3 | 00011100 >> 2                 == 11111111
        //(0x03 << 6) | (0x1f << 1) | (0x10 >> 4) == 00000011 << 6 | 00011111 << 1 | 00010000 >> 4 == 11111111
        //(0x0f << 4) | (0x1e >> 1)               == 00001111 << 4 | 00011110 >> 1                 == 11111111
        //(0x01 << 7) | (0x1f << 2) | (0x18 >> 3) == 00000001 << 7 | 00011111 << 2 | 00011000 >> 3 == 11111111
        //(0x07 << 5) | (0x1f)                    == 00000111 << 7 | 00011111                      == 11111111
        *pos++ = std::byte(((val0 & 0x1f) << 3) | ((val1 & 0x1c) >> 2));
        *pos++ = std::byte(((val1 & 0x03) << 6) | ((val2 & 0x1f) << 1) | ((val3 & 0x10) >> 4));
        *pos++ = std::byte(((val3 & 0x0f) << 4) | ((val4 & 0x1e) >> 1));
        *pos++ = std::byte(((val4 & 0x01) << 7) | ((val5 & 0x1f) << 2) | ((val6 & 0x18) >> 3));
        *pos++ = std::byte(((val6 & 0x07) << 5) | ((val7 & 0x1f)));
        in += 8;
    }
    assert(in == data.end()); //we've got trailing bytes
    auto padding{ 0u };
    while (in != data.begin() && *(--in) == '=') padding++;
    ret.resize((data.size() - padding) / 1.6);
    return ret;
}