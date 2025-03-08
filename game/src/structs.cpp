#ifndef STRUCTS_CPP
#define STRUCTS_CPP

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

using Entity = std::uint32_t;

#endif