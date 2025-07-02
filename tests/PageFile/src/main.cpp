#include <MSG/PageFile.hpp>

#include "./LoremIpsum.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iterator>
#include <vector>

#include <gtest/gtest.h>

using namespace MSG;

TEST(PageFile, HelloWorld)
{
    PageFile pageFile;
    std::string testString = "Hello World !";
    auto stringID          = pageFile.Allocate(testString.size());
    {
        std::vector<std::byte> data = { (std::byte*)std::to_address(testString.begin()), (std::byte*)std::to_address(testString.end()) };
        pageFile.Write(stringID, 0, data);
    }
    {
        auto memory = pageFile.Read(stringID, 0, testString.size());
        ASSERT_EQ(std::memcmp(memory.data(), testString.data(), testString.size()), 0);
        pageFile.Release(stringID);
        pageFile.Shrink();
    }
}

TEST(PageFile, MultiplePages)
{
    PageFile pageFile;
    std::vector<std::string> testStrings(4096);
    std::vector<PageID> testStringsID(4096);
    for (uint32_t i = 0; i < 4096; i++) {
        testStrings.at(i)   = "This is a test " + std::to_string(i);
        testStringsID.at(i) = pageFile.Allocate(testStrings.at(i).size());
        pageFile.Write(testStringsID.at(i), 0, { (std::byte*)std::to_address(testStrings.at(i).begin()), (std::byte*)std::to_address(testStrings.at(i).end()) });
    }
    bool error = false;
    for (uint32_t i = 0; i < 4096; i++) {
        auto memory = pageFile.Read(testStringsID.at(i), 0, testStrings.at(i).size());
        error |= std::memcmp(memory.data(), testStrings.at(i).data(), testStrings.at(i).size()) != 0;
        pageFile.Release(testStringsID.at(i));
    }
    pageFile.Shrink();
    ASSERT_EQ(error, false);
}

TEST(PageFile, MultiplePagesWithHoles)
{
    PageFile pageFile;
    std::vector<std::string> testStrings(4096);
    std::vector<PageID> testStringsID(4096);
    for (uint32_t i = 0; i < 4096; i++) {
        testStrings.at(i)   = "This is a test " + std::to_string(i);
        testStringsID.at(i) = pageFile.Allocate(testStrings.at(i).size());
        pageFile.Write(testStringsID.at(i), 0, { (std::byte*)std::to_address(testStrings.at(i).begin()), (std::byte*)std::to_address(testStrings.at(i).end()) });
    }
    for (uint32_t i = 0; i < 4096; i++) {
        if (i % 2 == 0)
            pageFile.Release(testStringsID.at(i));
    }
    {
        // allocate a space larger than a page
        auto stringID = pageFile.Allocate(loremIpsum.size());
        pageFile.Write(stringID, 0, { (std::byte*)std::to_address(loremIpsum.begin()), (std::byte*)std::to_address(loremIpsum.end()) });
        testStringsID.push_back(stringID);
    }
    for (uint32_t i = 0; i < 4096; i++) {
        if (i % 2 == 0) {
            testStringsID.at(i) = pageFile.Allocate(testStrings.at(i).size());
            pageFile.Write(testStringsID.at(i), 0, { (std::byte*)std::to_address(testStrings.at(i).begin()), (std::byte*)std::to_address(testStrings.at(i).end()) });
        }
    }
    bool error = false;
    for (uint32_t i = 0; i < 4096; i++) {
        auto memory = pageFile.Read(testStringsID.at(i), 0, testStrings.at(i).size());
        error |= std::memcmp(memory.data(), testStrings.at(i).data(), testStrings.at(i).size()) != 0;
        pageFile.Release(testStringsID.at(i));
    }
    pageFile.Shrink();
    ASSERT_EQ(error, false);
}

TEST(PageFile, LoremIpsum)
{
    PageFile pageFile;
    std::string testString = loremIpsum;
    auto stringID          = pageFile.Allocate(testString.size());
    pageFile.Write(stringID, 0, { (std::byte*)std::to_address(testString.begin()), (std::byte*)std::to_address(testString.end()) });
    auto memory = pageFile.Read(stringID, 0, testString.size());
    ASSERT_EQ(std::memcmp(memory.data(), testString.data(), testString.size()), 0);
    pageFile.Release(stringID);
    pageFile.Shrink();
}

TEST(PageFile, LoremIpsumWithOffset0)
{
    PageFile pageFile;
    constexpr auto offset  = 4100;
    std::string testString = loremIpsum;
    auto stringID          = pageFile.Allocate(testString.size());
    pageFile.Write(stringID, offset, { (std::byte*)std::to_address(testString.begin() + offset), (std::byte*)std::to_address(testString.end()) });
    auto testVec = pageFile.Read(stringID, offset, testString.size() - offset);
    pageFile.Release(stringID);
    pageFile.Shrink();
    ASSERT_EQ(std::memcmp(testVec.data(), testString.data() + offset, testVec.size()), 0);
}

TEST(PageFile, LoremIpsumWithOffset1)
{
    PageFile pageFile;
    constexpr auto offset    = 512;
    constexpr auto byteSize  = 512;
    constexpr auto chunkSize = 1024;
    std::string testString   = loremIpsum;
    auto stringID            = pageFile.Allocate(testString.size());
    pageFile.Write(stringID, offset, { (std::byte*)std::to_address(testString.begin() + offset), (std::byte*)std::to_address(testString.begin() + offset + byteSize) });
    auto testVec = pageFile.Read(stringID, offset, byteSize);
    pageFile.Release(stringID);
    pageFile.Shrink();
    ASSERT_EQ(std::memcmp(testVec.data(), testString.data() + offset, testVec.size()), 0);
}

TEST(PageFile, LoremIpsumWithOffset2)
{
    PageFile pageFile;
    constexpr auto offset    = 512;
    constexpr auto byteSize  = 4096;
    constexpr auto chunkSize = 4608;
    std::string testString   = { loremIpsum.begin(), loremIpsum.begin() + chunkSize };
    auto stringID            = pageFile.Allocate(testString.size());
    pageFile.Write(stringID, offset, { (std::byte*)std::to_address(testString.begin() + offset), (std::byte*)std::to_address(testString.begin() + offset + byteSize) });
    auto testVec = pageFile.Read(stringID, offset, byteSize);
    pageFile.Release(stringID);
    pageFile.Shrink();
    ASSERT_EQ(std::memcmp(testVec.data(), testString.data() + offset, testVec.size()), 0);
}

TEST(PageFile, LoremIpsumWithOffset3)
{
    PageFile pageFile;
    constexpr auto offset    = 24;
    constexpr auto byteSize  = 12;
    constexpr auto chunkSize = 72;
    std::string testString   = { loremIpsum.begin(), loremIpsum.begin() + chunkSize };
    auto stringID            = pageFile.Allocate(testString.size());
    pageFile.Write(stringID, offset, { (std::byte*)std::to_address(testString.begin() + offset), (std::byte*)std::to_address(testString.begin() + offset + byteSize) });
    auto testVec = pageFile.Read(stringID, offset, byteSize);
    pageFile.Release(stringID);
    pageFile.Shrink();
    ASSERT_EQ(std::memcmp(testVec.data(), testString.data() + offset, testVec.size()), 0);
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
