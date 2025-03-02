#ifndef ENGINE_CPP
#define ENGINE_CPP

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "../graphics/shader.h"
#include "EntityManager.cpp"
#include "ComponentManager.cpp"
#include "../graphics/RenderSystem.cpp"
#include "../graphics/TransformSystem.cpp"
#include "../graphics/Camera.cpp"
#include "scene.h"

class Engine {
    public:
        Engine() {

        };
        GLFWwindow* window;
        Shader* defaultShader;
        Shader* gridShader;
        EntityManager* entityManager;
        ComponentArray<TransformComponent>* transformComponents;
        ComponentArray<ModelComponent>* modelComponents;
        ComponentArray<EntityDataComponent>* entityDataComponents;
        RenderSystem* renderSystem;
        TransformSystem* transformSystem;
        Camera* camera;
        Scene* scene;
    };

#endif