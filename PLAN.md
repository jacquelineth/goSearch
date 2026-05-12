# Find4j C++ Port — Design Plan & Iteration Notes

## Source Project
Java project: `JavaSearch` (Find4j v0.3.0, Apache License 2.0, JavaTB Team)
Port date: May 2026

---

## Design Decisions

| Decision | Choice | Rationale |
|---|---|---|
| GUI framework | **Qt6** (Qt5 fallback) | QSettings, QCompleter, QDesktopServices, QSortFilterProxyModel, more active ecosystem than wxWidgets |
| Archive library | **miniz** (via FetchContent) | Single file, public domain, covers ZIP/JAR/WAR/EAR/ODS/ODT. No system dependency |
| Build system | **CMake 3.25+** | Cross-platform, native build file generation for Linux/Windows/macOS |
| C++ standard | **C++23** | std::expected, std::format, std::ranges::fold_left, std::println, std::jthread |
| Test framework | **GTest v1.14** (FetchContent) | Industry standard, minimal footprint |
| Search threading | **std::jthread** (not QThread) | Keeps search layer Qt-free; SearchListener bridges via queued Qt signals |
| Properties parser | Hand-rolled | Simple `key = value` format; no extra dependency needed |
| GZ/TAR support | **Not ported** | GZArchiveInputStream was an incomplete stub in Java |
| HtmlDocGenerator | **Not ported** | Not part of runtime functionality |

---

## Project Structure

```
cppSearch/
├── CMakeLists.txt                  # Top-level build
├── cmake/
│   └── Dependencies.cmake          # FetchContent: miniz 3.0.2, GTest v1.14
├── config/
│   ├── config.properties           # Archive extensions, history size, renderer class
│   └── logging.properties          # Log level, file output path
├── src/
│   ├── main_cli.cpp                # CLI entry: find4j-cli <root> <pattern> [<term>]
│   ├── main_gui.cpp                # GUI entry: find4j [rootPath]
│   ├── util/
│   │   ├── ConfigurationProperty.hpp   # enum class + constexpr key/default helpers
│   │   ├── StringUtils.hpp             # header-only toInt/toBoolean
│   │   ├── Configuration.hpp/.cpp      # Singleton lazy-load .properties parser
│   │   ├── Logger.hpp/.cpp             # std::format log formatter → file + stderr
│   │   ├── Persistable.hpp             # Abstract getValues/setValues interface
│   │   ├── PreferencesManager.hpp/.cpp # QSettings-backed persistence (JavaTB/Find4j)
│   │   └── FileUtils.hpp/.cpp          # FS I/O, extension helpers, desktop actions
│   ├── search/
│   │   ├── SearchElement.hpp/.cpp      # Immutable file/folder/archive descriptor
│   │   ├── SearchEngineEvent.hpp       # Event struct: Type enum + shared_ptr<element>
│   │   ├── SearchEngineListener.hpp    # Pure abstract observer interface
│   │   └── SearchEngine.hpp/.cpp       # Recursive search, wildcard→regex, archive traversal
│   ├── archive/
│   │   └── ArchiveHandler.hpp/.cpp     # miniz mz_zip_reader_* wrapper
│   └── ui/
│       ├── Theme.hpp/.cpp              # QPalette-based color themes
│       ├── AutoCompleter.hpp/.cpp      # QCompleter decorator base class
│       ├── SortedPersistedAutoCompleter.hpp/.cpp  # Sorted history + Persistable
│       ├── FramePersistenceHandler.hpp/.cpp        # saveGeometry/restoreGeometry
│       ├── SearchListener.hpp/.cpp     # SearchEngineListener → Qt queued signals
│       ├── UILauncher.hpp/.cpp         # QMainWindow: layout, search thread, slots
│       └── results/
│           ├── ResultRenderer.hpp          # Abstract interface
│           ├── ResultTableModel.hpp/.cpp   # QAbstractTableModel, 5 columns
│           ├── ResultItemDelegate.hpp/.cpp # Right-align SIZE/COMPRESSED_SIZE
│           ├── TableResultRenderer.hpp/.cpp # QTableView + QSortFilterProxyModel
│           ├── ListResultRenderer.hpp/.cpp  # QListView
│           ├── TablePersistenceHandler.hpp/.cpp # Column width persistence
│           └── actions/
│               ├── AbstractResultAction.hpp/.cpp
│               ├── OpenAction.hpp/.cpp
│               ├── EditAction.hpp/.cpp
│               ├── CopyToClipBoardAction.hpp/.cpp
│               ├── DeleteAction.hpp/.cpp
│               └── ExploreFileLocationAction.hpp/.cpp
└── test/
    ├── CMakeLists.txt
    ├── SearchElementTest.cpp
    ├── SearchEngineTest.cpp
    ├── ConfigurationTest.cpp
    ├── StringUtilsTest.cpp
    └── FileUtilsTest.cpp
```

---

## Implementation Phases

### Phase 1 — Build System
- `CMakeLists.txt`: C++23, Qt6 (Qt5 fallback), two executables, install rules, automoc/autouic
- `cmake/Dependencies.cmake`: FetchContent miniz 3.0.2 + GTest v1.14
- `find4j_core` static lib: util + search + archive + FileUtils (links Qt for desktop services)
- `find4j_qt_util` static lib: PreferencesManager (QSettings)
- `find4j-cli` executable: links find4j_core (no UI)
- `find4j` executable: WIN32/MACOSX_BUNDLE, links all

### Phase 2 — Utility Layer
- `ConfigurationProperty`: `enum class` with `constexpr propertyKey()` / `propertyDefault()`
- `StringUtils`: header-only, `std::from_chars` for safe int parsing
- `Configuration`: lazy singleton, hand-rolled `key = value` parser, searches `config/` relative to CWD
- `Logger`: `std::format` + `std::chrono`, `LOG_INFO/WARN/ERR` macros with `std::source_location`
- `Persistable`: `std::map<string,string>` round-trip interface
- `PreferencesManager`: `QSettings` group `JavaTB/Find4j`; `registerComponent()` auto-restores on registration
- `FileUtils`: `std::expected<string,string>` for I/O; `exploreSearchElement()` platform-branched (Explorer/Finder/xdg-open)

### Phase 3 — Core Search Engine
- `SearchElement`: 3 constructors (raw, `directory_entry`, `ArchiveEntryInfo`); `toString()` uses `std::ranges::fold_left`; Windows `FILE_ATTRIBUTE_HIDDEN` guarded by `#if defined(_WIN32)`
- `SearchEngine`: `std::atomic<bool> stopped_`; `std::mutex` on `currentPath_` deque; `wildcardToRegex()` converts `*`→`.*`, `?`→`.?`, `.`→`\\.`; content match guarded by 32 MB size limit
- `ArchiveHandler`: miniz `mz_zip_reader_init_mem` + `mz_zip_reader_extract_to_mem`; returns `std::expected<vector<ArchiveEntry>, string>`

### Phase 4 — CLI Entry Point
- Inline anonymous `CliListener` implements `SearchEngineListener`
- `std::println` for all output (C++23)
- Prints `--- N match(es) found ---` on END event

### Phase 5 — UI Layer (Qt6)
- `Theme`: 6 themes as `const Theme` statics; `applyTo(QWidget*)` sets `QPalette`
- `AutoCompleter`: `QCompleter` + `QStringListModel`; connects `QLineEdit::textEdited` signal
- `SortedPersistedAutoCompleter`: `std::vector<QString>` sorted copy; `std::format("item{}", i)` keys for persistence
- `SearchListener`: emits typed Qt signals (queued connection) for `matchFound`, `exploringPath`, `searchStarted`, `searchEnded`
- `UILauncher`: `std::thread` runs `SearchEngine::search()`; `QMetaObject::invokeMethod` replaced by direct queued signals; `closeEvent` saves all prefs
- `ResultTableModel`: `Qt::UserRole` returns raw `long long` for sort role (numeric sort for SIZE columns)
- `TableResultRenderer`: `QSortFilterProxyModel` with `setSortRole(Qt::UserRole)`; context menu built on demand
- `FramePersistenceHandler`: `saveGeometry/restoreGeometry` (Base64-encoded in QSettings)

### Phase 6 — Tests
- `SearchElementTest`: 7 tests covering all type flag combinations, toString(), ArchiveEntryInfo constructor
- `SearchEngineTest`: 5 tests including event sequence verification (START first, END last)
- `ConfigurationTest`: 6 tests for defaults, array property, int parsing, property key names
- `StringUtilsTest`: 9 tests for valid/invalid/boundary cases
- `FileUtilsTest`: 7 tests including write+read roundtrip via temp file

---

## C++23 Features Used

| Feature | Usage |
|---|---|
| `std::format` | Log messages, size/date formatting in ResultTableModel |
| `std::expected<T,E>` | FileUtils::readTextFile, writeTextFile; ArchiveHandler::readArchive |
| `std::ranges::fold_left` | SearchElement::toString() path joining |
| `std::ranges::sort`, `transform`, `any_of` | Sorting, case conversion, extension lookup |
| `std::filesystem` | Directory traversal, path manipulation, remove_all |
| `std::atomic<bool>` | SearchEngine stop flag |
| `std::mutex` + RAII lock_guard | SearchEngine path stack, Configuration |
| `std::string_view` | All read-only string parameters |
| `std::span<const std::byte>` | Archive data buffer views |
| `std::println` | CLI output |
| `std::source_location` | Logger macros capture file/line automatically |
| `std::from_chars` | StringUtils::toInt (no-throw, no-locale) |

---

## Iteration Notes

### Iteration 1 (initial generation)
- Created all 67 files in one pass.
- `find4j_core` initially excluded `FileUtils.cpp` (moved to `find4j_qt_util`).

### Iteration 2 (corrections)
- Moved `FileUtils.cpp` into `find4j_core` and linked Qt to core, so test target can link a single lib.
- Removed inline `#include "ArchiveHandler.hpp"` inside `FileUtils.cpp` namespace block; moved to top-level includes.
- Added missing `#include <filesystem>` in `DeleteAction.cpp`.
- Added missing `#include "../../util/FileUtils.hpp"` in `ListResultRenderer.cpp`.
- Added missing `#include "../../util/FileUtils.hpp"` in `TableResultRenderer.cpp`.
- Fixed `setToolTipText` → `setToolTip` in `UILauncher.cpp` (Qt method name).
- Added `#include <QDir>` and `#include <QFileInfo>` in `UILauncher.cpp`.
- Added `#if defined(_WIN32) / #include <windows.h>` guard at top of `SearchElement.cpp`.
- Test `CMakeLists.txt` updated to link Qt so `FileUtils::zipExtensions()` (uses Configuration) compiles.

---

## Known Limitations / Future Work

1. **GZ/TAR archives** — Not supported (Java stub was incomplete). Add `libarchive` and swap `ArchiveHandler` to enable.
2. **Theme switching at runtime** — Themes are defined but the UI is built with `Theme::DEFAULT` (system colors). A settings combo box could allow runtime theme selection.
3. **CPack packaging** — `cmake --install` works but no `.deb`/`.dmg`/NSIS installer yet. Add `include(CPack)` + platform-specific CPack variables.
4. **Qt Widgets → Qt Quick** — Could be migrated to QML/Qt Quick for a more modern look.
5. **Multiple root paths** — Supported via `:` (Linux/macOS) or `;` (Windows) separator in the path field, matching the Java behavior.
6. **miniz MD5 hash** — The FetchContent URL hash may need updating if a newer miniz release is used.
