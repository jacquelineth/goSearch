cmake_minimum_required(VERSION 3.25)

include(GoogleTest)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

set(TEST_SOURCES
    SearchElementTest.cpp
    SearchEngineTest.cpp
    ConfigurationTest.cpp
    StringUtilsTest.cpp
    FileUtilsTest.cpp
)

add_executable(find4j-tests ${TEST_SOURCES})

target_link_libraries(find4j-tests
    PRIVATE find4j_core
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE GTest::gtest_main
)

gtest_discover_tests(find4j-tests)
