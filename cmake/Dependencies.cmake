# cmake/Dependencies.cmake
include(FetchContent)

# ─── miniz (amalgamated single-file ZIP library, public domain) ──────────────
# We fetch the amalgamated miniz release tarball so there is exactly one .h and
# one .c file, no additional headers required.
FetchContent_Declare(
    miniz_fetch
    URL      https://github.com/richgel999/miniz/releases/download/3.0.2/miniz-3.0.2.zip
    URL_HASH MD5=7db6ad2b1d9d07e47c20ac2a8a11e2f7
)
FetchContent_MakeAvailable(miniz_fetch)

# ─── GTest for unit tests ─────────────────────────────────────────────────────
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
