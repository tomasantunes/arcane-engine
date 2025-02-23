cmake_minimum_required(VERSION 3.10)
project(ArcaneEngine)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Include directories
include_directories(
    imgui
    imgui/backends
    /usr/include
)

# Source files
set(SOURCES
    src/main.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

# Executable
add_executable(ArcaneEngine ${SOURCES})

# Link libraries
target_link_libraries(ArcaneEngine PRIVATE glfw OpenGL::GL GLU GLEW)
