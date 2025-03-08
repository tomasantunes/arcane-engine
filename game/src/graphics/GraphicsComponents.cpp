#ifndef GRAPHICSCOMPONENTS_CPP
#define GRAPHICSCOMPONENTS_CPP

#include <glm/glm.hpp>
#include "model.h"

struct TransformComponent {
    glm::mat4 transform;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct ModelComponent {
    Model* model;
};

struct PointLightComponent {
    glm::vec3 position;
    glm::vec3 color;
};

#endif