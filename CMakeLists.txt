cmake_minimum_required(VERSION 3.26)

# ---- Project ----
project(file-parser
    VERSION 0.1.0
    LANGUAGES CXX
)

# ---- Options ----
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# vcpkg debug helper (optional)
# message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")

# ---- Warnings (Platform aware) ----
if (MSVC)
    add_compile_options(/W4 /permissive- /EHsc)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---- Collect all sources recursively ----
file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE HEADERS
    "${PROJECT_SOURCE_DIR}/src/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.hpp"
)

# ---- Target ----
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
)

# ---- Example: link external libs later ----
# target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 imgui::imgui fmt::fmt)
