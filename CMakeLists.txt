include(FetchContent)

cmake_minimum_required(VERSION 3.26)

project(pz-map-generator VERSION 0.1.0 LANGUAGES CXX)

# Settings
set(FETCHCONTENT_BASE_DIR  "${CMAKE_SOURCE_DIR}/dependencies")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
find_package(fmt CONFIG REQUIRED)
find_package(cpptrace CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)
find_package(lodepng CONFIG REQUIRED)
find_package(SFML COMPONENTS System Graphics Window CONFIG REQUIRED)

# TGUI
set(TGUI_BACKEND SFML_GRAPHICS)
FetchContent_Declare(
    TGUI
    GIT_REPOSITORY https://github.com/texus/TGUI.git
    GIT_TAG        v1.11.0
)
FetchContent_MakeAvailable(tgui)

#CLI11
include(FetchContent)
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG        v2.6.1
)
FetchContent_MakeAvailable(cli11)

# Library
file(GLOB_RECURSE LIB_SOURCES "lib/*.cpp")
add_library(pzlib STATIC ${LIB_SOURCES})
target_include_directories(pzlib PUBLIC lib main)
target_link_libraries(pzlib PUBLIC
    fmt::fmt
    cpptrace::cpptrace
    lodepng
    SFML::System
    SFML::Graphics
    SFML::Window
    CLI11::CLI11
    tgui
    dwmapi
)

# Main executable
file(GLOB_RECURSE MAIN_SOURCES "main/*.cpp")
add_executable(${PROJECT_NAME} ${MAIN_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE pzlib)

# Test executable
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
add_executable(tests ${TEST_SOURCES})
target_include_directories(pzlib PUBLIC lib)
target_link_libraries(tests PRIVATE pzlib doctest::doctest)
