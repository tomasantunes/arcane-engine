#ifndef TRANSFORMSYSTEM_CPP
#define TRANSFORMSYSTEM_CPP

#include "../general/System.cpp"
#include "../general/ComponentManager.cpp"
#include "GraphicsComponents.cpp"

class TransformSystem : public System {
    public:
        ComponentArray<TransformComponent>* transformArray;

        void Update(float dt) override {
            
        }

        void SetTransform() {
            for (Entity entity : entities) {
                UpdateEntityTransform(entity);
            }
        }

        void UpdateEntityTransform(Entity entity) {
            if (entities.find(entity) != entities.end()) {
                TransformComponent* transform = transformArray->GetComponent(entity);
                if (transform) {
                    transform->transform = glm::translate(glm::mat4(1.0f), transform->position);
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
                    transform->transform = glm::scale(
                        transform->transform,
                        transform->scale
                    );
                }
            }
        }
};

#endif