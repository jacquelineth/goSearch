// C++ port - SearchElement unit tests
#include <gtest/gtest.h>
#include "../src/search/SearchElement.hpp"

using namespace javatb;

TEST(SearchElement, TypeFlagsFile) {
    SearchElement e("test.txt", SearchElement::FILE, {});
    EXPECT_TRUE(e.isFile());
    EXPECT_FALSE(e.isFolder());
    EXPECT_FALSE(e.isArchive());
    EXPECT_FALSE(e.isArchiveEntry());
}

TEST(SearchElement, TypeFlagsFolder) {
    SearchElement e("/tmp", SearchElement::FOLDER, {});
    EXPECT_FALSE(e.isFile());
    EXPECT_TRUE(e.isFolder());
    EXPECT_FALSE(e.isArchive());
    EXPECT_FALSE(e.isArchiveEntry());
}

TEST(SearchElement, TypeFlagsArchiveFile) {
    SearchElement e("archive.zip", SearchElement::ARCHIVE | SearchElement::FILE, {});
    EXPECT_TRUE(e.isFile());
    EXPECT_FALSE(e.isFolder());
    EXPECT_TRUE(e.isArchive());
    EXPECT_FALSE(e.isArchiveEntry());
}

TEST(SearchElement, TypeFlagsArchiveEntry) {
    SearchElement e("entry.txt", SearchElement::ENTRY, {});
    EXPECT_FALSE(e.isFile());
    EXPECT_FALSE(e.isFolder());
    EXPECT_FALSE(e.isArchive());
    EXPECT_TRUE(e.isArchiveEntry());
}

TEST(SearchElement, ToStringNoPaths) {
    SearchElement e("hello.txt", SearchElement::FILE, {});
    EXPECT_EQ(e.toString(), "hello.txt");
}

TEST(SearchElement, ToStringWithPath) {
    auto parent = std::make_shared<SearchElement>("archive.zip",
        SearchElement::ARCHIVE | SearchElement::FILE, SearchElement::PathList{});
    SearchElement::PathList path{parent};
    SearchElement e("entry.txt", SearchElement::ENTRY, path);
    std::string result = e.toString();
    EXPECT_TRUE(result.find("archive.zip") != std::string::npos);
    EXPECT_TRUE(result.ends_with("entry.txt"));
}

TEST(SearchElement, FromArchiveInfo) {
    SearchElement::ArchiveEntryInfo info{"dir/file.txt", 1024, 512, -1LL};
    SearchElement e(info, SearchElement::ENTRY, {});
    EXPECT_EQ(e.getName(), "dir/file.txt");
    EXPECT_EQ(e.getSize(), 1024LL);
    EXPECT_EQ(e.getCompressedSize(), 512LL);
    EXPECT_FALSE(e.hasLastModified());
    EXPECT_EQ(e.getAttributes(), "R...");
}

TEST(SearchElement, DefaultMetadata) {
    SearchElement e("x", 0, {});
    EXPECT_EQ(e.getSize(), -1LL);
    EXPECT_EQ(e.getCompressedSize(), -1LL);
    EXPECT_FALSE(e.hasLastModified());
    EXPECT_EQ(e.getAttributes(), "");
}
