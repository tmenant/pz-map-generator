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

# Pour aider à debugger vcpkg (optionnel)
# message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")

# ---- Warnings (Platform aware) ----
if (MSVC)
    add_compile_options(/W4 /permissive- /EHsc)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---- Target ----
add_executable(${PROJECT_NAME}
    src/main.cpp
)

# include headers
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# ---- Example: link external libs later ----
# target_link_libraries(zomboid-rwg PRIVATE SDL2::SDL2 imgui::imgui fmt::fmt)