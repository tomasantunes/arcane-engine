cmake_minimum_required(VERSION 3.10)
project(ArcaneEngine)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

# Include directories
include_directories(
    imgui
    imgui/backends
    /usr/include
)

# Source files
set(SOURCES
    src/main.cpp
    src/constants.cpp
    src/structs.cpp
    src/general/EntityManager.cpp
    src/general/ComponentManager.cpp
    src/general/System.cpp
    src/general/scene.h
    src/general/scene.cpp
    src/graphics/model.h
    src/graphics/model.cpp
    src/graphics/shader.h
    src/graphics/shader.cpp
    src/graphics/GraphicsComponents.cpp
    src/graphics/RenderSystem.cpp
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
target_link_libraries(ArcaneEngine PRIVATE glfw OpenGL::GL GLU GLEW assimp::assimp)

# Create a 'shaders' directory in the build folder
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

# Copy GLSL files to the 'shaders' directory in the build folder
file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/src/graphics/shaders/*.glsl")
foreach(SHADER_FILE ${SHADER_FILES})
    # Extract the filename (e.g., vertex_shader.glsl)
    get_filename_component(SHADER_FILENAME ${SHADER_FILE} NAME)

    # Configure the file to be copied to the 'shaders' directory
    configure_file(${SHADER_FILE} ${CMAKE_BINARY_DIR}/shaders/${SHADER_FILENAME} COPYONLY)
endforeach()
