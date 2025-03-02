#ifndef RENDERSYSTEM_CPP
#define RENDERSYSTEM_CPP

#include "../general/System.cpp"
#include "../general/ComponentManager.cpp"
#include "GraphicsComponents.cpp"
#include "model.h"
#include "shader.h"
#include "Camera.cpp"

class RenderSystem : public System {
    public:
        ComponentArray<TransformComponent>* transformArray;
        ComponentArray<ModelComponent>* modelArray;
        ComponentArray<PointLightComponent>* pointLightArray;
        Shader* shader;
        Shader* gridShader;
        Shader* lightShader;
        Camera* camera;
        glm::vec2 size;
        std::vector<float> gridVertices;
        GLuint gridVAO, gridVBO;
        GLuint lightVAO, lightVBO;
    
        void Update(float dt) override {
            glm::vec3 lightPos;
            glm::vec3 lightColor;
            PointLightComponent* light;

            for (Entity entity : entities) {
                light = pointLightArray->GetComponent(entity);

                if (light) {
                    lightPos = light->position;
                    lightColor = light->color;
                    break;
                }
            }

            
            glm::vec3 objectColor(1.0f, 0.5f, 0.2f); // Orange object

            glUseProgram(shader->Program);
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = camera->GetViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)size.x / (float)size.y, 0.1f, 100.0f);
            
            GLint modelLoc = glGetUniformLocation(shader->Program, "model");
            GLint viewLoc = glGetUniformLocation(shader->Program, "view");
            GLint projLoc = glGetUniformLocation(shader->Program, "projection");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

            shader->setVec3("lightPos", lightPos);
            shader->setVec3("lightColor", lightColor);
            shader->setVec3("objectColor", objectColor);

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

        void LoadGrid() {
            int gridSize = 1000;
            float spacing = 1.0f;
            for (int i = -gridSize; i <= gridSize; ++i) {
                gridVertices.push_back(-gridSize * spacing); gridVertices.push_back(0.0f); gridVertices.push_back(i * spacing);
                gridVertices.push_back(gridSize * spacing); gridVertices.push_back(0.0f); gridVertices.push_back(i * spacing);
                gridVertices.push_back(i * spacing); gridVertices.push_back(0.0f); gridVertices.push_back(-gridSize * spacing);
                gridVertices.push_back(i * spacing); gridVertices.push_back(0.0f); gridVertices.push_back(gridSize * spacing);
            }

            glGenVertexArrays(1, &gridVAO);
            glGenBuffers(1, &gridVBO);
            glBindVertexArray(gridVAO);
            glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
            glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void LoadLight() {
            float vertices[] = {
                // Horizontal line
                -0.5f,  0.0f, 0.0f,
                 0.5f,  0.0f, 0.0f,
        
                // Vertical line
                 0.0f, -0.5f, 0.0f,
                 0.0f,  0.5f, 0.0f
            };

            glGenVertexArrays(1, &lightVAO);
            glGenBuffers(1, &lightVBO);

            glBindVertexArray(lightVAO);
            glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            // Set up vertex attribute pointers
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }

        void DrawGrid(float dt) {
            glUseProgram(gridShader->Program);
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = camera->GetViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)size.x / (float)size.y, 0.1f, 100.0f);
            gridShader->setMat4("model", model);
            gridShader->setMat4("view", view);
            gridShader->setMat4("projection", projection);

            // Draw the grid
            glBindVertexArray(gridVAO);
            glDrawArrays(GL_LINES, 0, gridVertices.size() / 3);
            glBindVertexArray(0);
        }

        void DrawLight(float dt) {
            PointLightComponent* light;

            for (Entity entity : entities) {
                light = pointLightArray->GetComponent(entity);

                if (light) {
                    glUseProgram(lightShader->Program);
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), light->position);
                    glm::mat4 view = camera->GetViewMatrix();
                    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)size.x / (float)size.y, 0.1f, 100.0f);
                    lightShader->setMat4("model", model);
                    lightShader->setMat4("view", view);
                    lightShader->setMat4("projection", projection);

                    glBindVertexArray(lightVAO);
                    glDrawArrays(GL_LINES, 0, 4);
                    break;
                }
            }
            
        }
};

#endif