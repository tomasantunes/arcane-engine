#ifndef TRANSFORMSYSTEM_CPP
#define TRANSFORMSYSTEM_CPP

#include "../general/System.cpp"
#include "../general/ComponentManager.cpp"
#include "GraphicsComponents.cpp"

class TransformSystem : public System {
    public:
        ComponentArray<TransformComponent>* transformArray;

        void Update(float dt) override {
            for (Entity entity : entities) {
                TransformComponent* transform = transformArray->GetComponent(entity);

                if (transform) {
                    transform->transform = glm::translate(transform->transform, transform->position);
                    transform->position = glm::vec3(0.0f, 0.0f, 0.0f);
                    transform->transform = glm::rotate(
                        transform->transform,
                        glm::radians(transform->rotation.x),
                        glm::vec3(1.0f, 0.0f, 0.0f)
                    );
                    transform->transform = glm::rotate(
                        transform->transform,
                        glm::radians(transform->rotation.y),
                        glm::vec3(0.0f, 1.0f, 0.0f)
                    );
                    transform->transform = glm::rotate(
                        transform->transform,
                        glm::radians(transform->rotation.z),
                        glm::vec3(0.0f, 0.0f, 1.0f)
                    );
                    transform->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                    transform->transform = glm::scale(
                        transform->transform,
                        transform->scale
                    );
                    transform->scale = glm::vec3(1.0f, 1.0f, 1.0f);
                }
            }
        }
};

#endif