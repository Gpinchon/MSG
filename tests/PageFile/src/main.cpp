#include <MSG/PageFile.hpp>

#include "./LoremIpsum.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iterator>
#include <vector>

#include <gtest/gtest.h>

using namespace MSG;

TEST(Assets, PageFile)
{
    PageFile pageFile;
    {
        std::string testString = "Hello World !";
        auto stringID          = pageFile.Allocate(testString.size());
        pageFile.Write(stringID, { (std::byte*)std::to_address(testString.begin()), (std::byte*)std::to_address(testString.end()) });
        auto testVec = pageFile.Read(stringID, testString.size());
        pageFile.Release(stringID);
        pageFile.Shrink();
        ASSERT_EQ(std::memcmp(testVec.data(), testString.data(), testString.size()), 0);
    }
    {
        std::vector<std::string> testStrings(4096);
        std::vector<PageID> testStringsID(4096);
        for (uint32_t i = 0; i < 4096; i++) {
            testStrings.at(i)   = "This is a test " + std::to_string(i);
            testStringsID.at(i) = pageFile.Allocate(testStrings.at(i).size());
            pageFile.Write(testStringsID.at(i), { (std::byte*)std::to_address(testStrings.at(i).begin()), (std::byte*)std::to_address(testStrings.at(i).end()) });
        }
        bool error = false;
        for (uint32_t i = 0; i < 4096; i++) {
            auto testVec = pageFile.Read(testStringsID.at(i), testStrings.at(i).size());
            pageFile.Release(testStringsID.at(i));
            error |= std::memcmp(testVec.data(), testStrings.at(i).data(), testStrings.at(i).size()) != 0;
        }
        pageFile.Shrink();
        ASSERT_EQ(error, false);
    }
    {
        std::string testString = loremIpsum;
        auto stringID          = pageFile.Allocate(testString.size());
        pageFile.Write(stringID, { (std::byte*)std::to_address(testString.begin()), (std::byte*)std::to_address(testString.end()) });
        auto testVec = pageFile.Read(stringID, testString.size());
        pageFile.Release(stringID);
        pageFile.Shrink();
        ASSERT_EQ(std::memcmp(testVec.data(), testString.data(), testString.size()), 0);
    }
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
