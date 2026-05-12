cmake_minimum_required(VERSION 3.25)
project(Find4j VERSION 0.3.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt automoc/autouic/autorcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include(cmake/Dependencies.cmake)

# ─── miniz static library (built from FetchContent) ─────────────────────────
# miniz_fetch_SOURCE_DIR is populated by FetchContent_MakeAvailable in Dependencies.cmake
add_library(miniz STATIC
    ${miniz_fetch_SOURCE_DIR}/miniz.c
)
target_include_directories(miniz PUBLIC ${miniz_fetch_SOURCE_DIR})
set_target_properties(miniz PROPERTIES C_STANDARD 99)

# ─── core (no Qt) ─────────────────────────────────────────────────────────
set(CORE_SOURCES
    src/util/Configuration.cpp
    src/util/Logger.cpp
    src/util/FileUtils.cpp
    src/archive/ArchiveHandler.cpp
    src/search/SearchElement.cpp
    src/search/SearchEngine.cpp
)

add_library(find4j_core STATIC ${CORE_SOURCES})
target_include_directories(find4j_core PUBLIC src)
target_link_libraries(find4j_core PUBLIC miniz)
if(WIN32)
    target_compile_definitions(find4j_core PUBLIC NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

# ─── util_qt (Qt-dependent utilities) ────────────────────────────────────
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Concurrent)

# Propagate QT availability so FileUtils.cpp can guard Qt-only code
target_compile_definitions(find4j_core PUBLIC FIND4J_HAS_QT)
target_link_libraries(find4j_core PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)

set(QT_UTIL_SOURCES
    src/util/PreferencesManager.cpp
)

add_library(find4j_qt_util STATIC ${QT_UTIL_SOURCES})
target_include_directories(find4j_qt_util PUBLIC src)
target_link_libraries(find4j_qt_util
    PUBLIC find4j_core
    PUBLIC Qt${QT_VERSION_MAJOR}::Core
    PUBLIC Qt${QT_VERSION_MAJOR}::Widgets
)

# ─── CLI executable ───────────────────────────────────────────────────────
add_executable(find4j-cli src/main_cli.cpp)
target_link_libraries(find4j-cli PRIVATE find4j_core)
if(WIN32)
    # console subsystem for CLI
    set_target_properties(find4j-cli PROPERTIES WIN32_EXECUTABLE FALSE)
endif()

# ─── GUI executable ───────────────────────────────────────────────────────
set(GUI_SOURCES
    src/main_gui.cpp
    src/ui/Theme.cpp
    src/ui/AutoCompleter.cpp
    src/ui/SortedPersistedAutoCompleter.cpp
    src/ui/FramePersistenceHandler.cpp
    src/ui/SearchListener.cpp
    src/ui/UILauncher.cpp
    src/ui/results/ResultTableModel.cpp
    src/ui/results/ResultItemDelegate.cpp
    src/ui/results/TableResultRenderer.cpp
    src/ui/results/ListResultRenderer.cpp
    src/ui/results/TablePersistenceHandler.cpp
    src/ui/results/actions/AbstractResultAction.cpp
    src/ui/results/actions/OpenAction.cpp
    src/ui/results/actions/EditAction.cpp
    src/ui/results/actions/CopyToClipBoardAction.cpp
    src/ui/results/actions/DeleteAction.cpp
    src/ui/results/actions/ExploreFileLocationAction.cpp
)

if(WIN32)
    add_executable(find4j WIN32 ${GUI_SOURCES})
elseif(APPLE)
    add_executable(find4j MACOSX_BUNDLE ${GUI_SOURCES})
else()
    add_executable(find4j ${GUI_SOURCES})
endif()

target_link_libraries(find4j
    PRIVATE find4j_qt_util
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent
)

# ─── Tests ────────────────────────────────────────────────────────────────
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# ─── Install ──────────────────────────────────────────────────────────────
include(GNUInstallDirs)
install(TARGETS find4j find4j-cli
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY config/
    DESTINATION ${CMAKE_INSTALL_BINDIR}/config
)
install(DIRECTORY images/
    DESTINATION ${CMAKE_INSTALL_BINDIR}/images
    OPTIONAL
)
