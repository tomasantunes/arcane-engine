#ifndef RENDERSYSTEM_CPP
#define RENDERSYSTEM_CPP

#include "../general/System.cpp"
#include "../general/ComponentManager.cpp"
#include "GraphicsComponents.cpp"
#include "model.h"
#include "shader.h"

class RenderSystem : public System {
    public:
        ComponentArray<TransformComponent>* transformArray;
        ComponentArray<ModelComponent>* modelArray;
        Shader* shader;
    
        void Update(float dt) override {
            for (Entity entity : entities) {
                TransformComponent* transform = transformArray->GetComponent(entity);
                ModelComponent* model = modelArray->GetComponent(entity);
    
                if (transform) {
                    shader->setMat4("model", transform->transform);
                    glBindVertexArray(model->model->VAO);
                    glDrawElements(GL_TRIANGLES, model->model->indices.size(), GL_UNSIGNED_INT, 0);
                    glBindVertexArray(0);
                }
            }
        }
};

#endif