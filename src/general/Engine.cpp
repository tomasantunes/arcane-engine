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
#include "ScriptComponent.cpp"
#include "scene.h"

class Engine {
    public:
        Engine() {

        };
        GLFWwindow* window;
        Shader* defaultShader;
        Shader* gridShader;
        Shader* lightShader;
        EntityManager* entityManager;
        ComponentArray<TransformComponent>* transformComponents;
        ComponentArray<ModelComponent>* modelComponents;
        ComponentArray<EntityDataComponent>* entityDataComponents;
        ComponentArray<ScriptComponent>* scriptComponents;
        ComponentArray<PointLightComponent>* pointLightComponents;
        RenderSystem* renderSystem;
        TransformSystem* transformSystem;
        Camera* camera;
        Scene* scene;
        std::string mode;
    };

#endif