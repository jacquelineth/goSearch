// C++ port - FileUtils unit tests (no Qt dependency required)
#include <gtest/gtest.h>
#include "../src/util/FileUtils.hpp"

using namespace javatb;

TEST(FileUtils, GetFileExtensionByString) {
    EXPECT_EQ(FileUtils::getFileExtension("archive.ZIP"), "zip");
    EXPECT_EQ(FileUtils::getFileExtension("file.txt"),    "txt");
    EXPECT_EQ(FileUtils::getFileExtension("noext"),       "");
    EXPECT_EQ(FileUtils::getFileExtension(".hidden"),     "");
    EXPECT_EQ(FileUtils::getFileExtension("path/to/file.java"), "java");
}

TEST(FileUtils, GetFileExtensionArchivePathSeparator) {
    // Extension after path separator should still work
    EXPECT_EQ(FileUtils::getFileExtension("a/b.zip"), "zip");
    EXPECT_EQ(FileUtils::getFileExtension("a\\b.jar"), "jar");
}

TEST(FileUtils, GetFileShortName) {
    EXPECT_EQ(FileUtils::getFileShortName("a/b/c.txt"),  "c.txt");
    EXPECT_EQ(FileUtils::getFileShortName("a\\b\\c.txt"), "c.txt");
    EXPECT_EQ(FileUtils::getFileShortName("file.txt"),   "file.txt");
    EXPECT_EQ(FileUtils::getFileShortName("/root/f"),    "f");
}

TEST(FileUtils, ZipExtensionsContainsCommonArchives) {
    const auto& exts = FileUtils::zipExtensions();
    EXPECT_TRUE(exts.contains("zip"));
    EXPECT_TRUE(exts.contains("jar"));
    EXPECT_TRUE(exts.contains("war"));
    EXPECT_TRUE(exts.contains("ear"));
}

TEST(FileUtils, ReadTextFileMissing) {
    auto result = FileUtils::readTextFile("__nonexistent_file__.txt");
    EXPECT_FALSE(result.has_value());
}

TEST(FileUtils, WriteAndReadTextFile) {
    namespace fs = std::filesystem;
    auto tmp = fs::temp_directory_path() / "find4j_test_file.txt";
    auto writeResult = FileUtils::writeTextFile(tmp, "hello\nworld\n");
    ASSERT_TRUE(writeResult.has_value());

    auto readResult = FileUtils::readTextFile(tmp);
    ASSERT_TRUE(readResult.has_value());
    EXPECT_EQ(*readResult, "hello\nworld\n");

    fs::remove(tmp);
}
