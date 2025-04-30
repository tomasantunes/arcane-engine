cmake_minimum_required(VERSION 3.10)
project(ArcaneEngine)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(Lua 5.3 REQUIRED)



# Include directories
include_directories(
    /usr/include
)

include_directories(${CMAKE_SOURCE_DIR}/lib/imgui)
include_directories(${CMAKE_SOURCE_DIR}/lib/imgui/backends)
include_directories(${CMAKE_SOURCE_DIR}/lib/ImGuiFileDialog)
include_directories(${LUA_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/lib)


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
    src/general/GameData.cpp
    src/general/ScriptSystem.h
    src/general/ScriptSystem.cpp
    src/general/Script.h
    src/general/Script.cpp
    src/general/ScriptComponent.cpp
    src/general/EntityDataComponent.cpp
    src/general/API.cpp
    src/general/Engine.cpp
    src/graphics/model.h
    src/graphics/model.cpp
    src/graphics/shader.h
    src/graphics/shader.cpp
    src/graphics/GraphicsComponents.cpp
    src/graphics/RenderSystem.cpp
    src/graphics/Camera.cpp
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/backends/imgui_impl_glfw.h
    lib/imgui/backends/imgui_impl_glfw.cpp
    lib/imgui/backends/imgui_impl_opengl3.h
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/ImGuiFileDialog/ImGuiFileDialog.h
    lib/ImGuiFileDialog/ImGuiFileDialog.cpp
    lib/sol/config.hpp
    lib/sol/forward.hpp
    lib/sol/sol.hpp
)

# Executable
add_executable(ArcaneEngine ${SOURCES})

# Link libraries
target_link_libraries(ArcaneEngine PRIVATE glfw OpenGL::GL GLU GLEW assimp::assimp ${LUA_LIBRARIES})

file(COPY game DESTINATION .)
file(COPY src DESTINATION ./game)
file(COPY lib DESTINATION ./game)

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

