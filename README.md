# Find4j — C++ Port

Cross-platform C++ port of the Find4j file-search utility.

## Requirements

| Dependency | Version |
|---|---|
| C++ Compiler | C++23 support (GCC 13+, Clang 16+, MSVC 19.35+) |
| CMake | 3.25+ |
| Qt | 6.x (Qt 5.15 fallback supported) |

Qt must be installed and visible to CMake via `CMAKE_PREFIX_PATH` or the system package manager.

## Quick Build

### Linux / macOS

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

### Windows (MSVC)

```cmd
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:\Qt\6.x.x\msvc2022_64
cmake --build build --config Release
```

### Windows (MinGW)

```cmd
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:\Qt\6.x.x\mingw_64
cmake --build build
```

### macOS (Homebrew Qt)

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$(brew --prefix qt)
cmake --build build -j$(sysctl -n hw.ncpu)
```

## Run

### GUI

```sh
./build/find4j [rootPath]
```

### CLI

```sh
./build/find4j-cli <rootPath> <filePattern> [<searchString>]
# Examples:
./build/find4j-cli /home/user "*.java" "SearchEngine"
./build/find4j-cli /tmp "*.log"
```

## Run Tests

```sh
cmake -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Install

```sh
cmake --install build --prefix /usr/local
```

This copies the executables and the `config/` directory to the install prefix.

## Configuration

`config/config.properties` — edit before running:

```properties
# Archive extensions to search inside (space-separated)
org.javatb.search.zip.extensions = zip jar war ear rar pak ods odp odg odt

# Auto-complete history size
org.javatb.history.size = 10

# Result renderer: TableResultRenderer or ListResultRenderer
org.javatb.renderer.class = TableResultRenderer
```

## Architecture

```
cppSearch/
├── CMakeLists.txt          # Top-level build (C++23, Qt6/5, miniz via FetchContent)
├── cmake/Dependencies.cmake
├── config/                 # Runtime config (copied next to executable on install)
├── src/
│   ├── main_cli.cpp        # CLI entry point
│   ├── main_gui.cpp        # GUI entry point
│   ├── util/               # Configuration, Logger, FileUtils, PreferencesManager
│   ├── search/             # SearchEngine, SearchElement, events, listener interface
│   ├── archive/            # ArchiveHandler (miniz wrapper for ZIP/JAR/WAR/EAR)
│   └── ui/                 # Qt6 GUI: UILauncher, results table/list, actions, themes
└── test/                   # GTest unit tests (SearchElement, Engine, Config, Utils)
```

## Notes

- **miniz** is fetched automatically by CMake (FetchContent) from the official GitHub release.
- **GTest** is fetched automatically (test builds only).
- Window position, column widths, and search history persist via `QSettings` (`JavaTB/Find4j`).
- Supports nested archives (archives within archives).
