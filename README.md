# GoSearch

This repository contains a Go port of the JavaSearch / Find4j project.
It provides a command-line search tool and a lightweight GUI that reuse the original Java `config/config.properties` and image assets.

## Prerequisites

- Go 1.20 or later
- On Linux, install native GUI libraries for Fyne, for example:
  - `libx11-dev`, `libxrandr-dev`, `libxcursor-dev`, `libxinerama-dev`, `libxi-dev`, `libxxf86vm-dev`

## Build

On Linux/macOS:

```bash
./build.sh
```

On Windows:

```cmd
build.cmd
```

## Usage

Command-line search:

```bash
./bin/search <rootPath> <filePattern> [<searchString>]
```

GUI search:

```bash
./bin/searchui [<rootPath>]
```

If `<rootPath>` is omitted in the GUI, the application starts in the current working directory.

## Resources

The Go port reuses:

- `config/config.properties`
- `images/JavaTB-icon.gif`
- other Java image assets copied from the original project

## Notes

The search engine supports nested archives and wildcards (`*` and `?`) in file names, matching the Java implementation.
