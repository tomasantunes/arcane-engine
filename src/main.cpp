#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <sol/sol.hpp>
#include "structs.cpp"
#include "constants.cpp"
#include "general/Engine.cpp"
#include "general/EntityManager.cpp"
#include "general/ComponentManager.cpp"
#include "general/EntityDataComponent.cpp"
#include "general/ScriptComponent.cpp"
#include "general/GameData.cpp"
#include "general/scene.h"
#include "graphics/model.h"
#include "graphics/shader.h"
#include "graphics/GraphicsComponents.cpp"
#include "graphics/RenderSystem.cpp"
#include "graphics/TransformSystem.cpp"
#include "graphics/Camera.cpp"

namespace fs = std::filesystem;

int size_w = 1280;
int size_h = 768;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool cameraActive = false; // Tracks whether the camera is active
bool cursorLocked = false; // Tracks whether the cursor is locked
double lastMouseX = 0.0, lastMouseY = 0.0; // Stores the last mouse position
static int selected_entity_idx = 0;
std::string add_script_name = "";
Entity selected_entity = 0;
Engine engine;

void setupGLFW();
GLFWwindow* createWindow(int width, int height, const char* title);
void setupImGui(GLFWwindow* window);
void renderLoop(Engine engine);

GLuint fbo, fboTexture, rbo;
void setupFramebuffer(int width, int height) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void toggleCamera(GLFWwindow* window, bool activate) {
    cameraActive = activate;
    cursorLocked = activate;

    if (activate) {
        // Hide and lock the cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Store the initial mouse position
        glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
    } else {
        // Show and unlock the cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}


int main() {
    setupGLFW();
    GLFWwindow* window = createWindow(size_w, size_h, "ArcaneEngine");
    if (!window) return -1;
    engine.window = window;

    setupImGui(engine.window);

    setupFramebuffer(500, 380);

    sol::state lua;
    lua.open_libraries(sol::lib::base);
    lua.script("print('Hello from fixed setup!')");

    engine.mode = "editor";
    Camera camera;
    Scene scene;
    engine.camera = &camera;
    engine.camera->SetTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    engine.scene = &scene;

    Shader defaultShader("shaders/default-vshader.glsl", "shaders/default-fshader.glsl");
    Shader gridShader("shaders/grid-vshader.glsl", "shaders/grid-fshader.glsl");
    Shader lightShader("shaders/light-vshader.glsl", "shaders/light-fshader.glsl");

    engine.defaultShader = &defaultShader;
    engine.gridShader = &gridShader;
    engine.lightShader = &lightShader;

    EntityManager entityManager;
    ComponentArray<TransformComponent> transformComponents;
    ComponentArray<ModelComponent> modelComponents;
    ComponentArray<EntityDataComponent> entityDataComponents;
    ComponentArray<PointLightComponent> pointLightComponents;
    ComponentArray<ScriptComponent> scriptComponents;
    engine.entityManager = &entityManager;
    engine.transformComponents = &transformComponents;
    engine.modelComponents = &modelComponents;
    engine.entityDataComponents = &entityDataComponents;
    engine.pointLightComponents = &pointLightComponents;
    engine.scriptComponents = &scriptComponents;

    RenderSystem renderSystem;
    engine.renderSystem = &renderSystem;
    engine.renderSystem->transformArray = engine.transformComponents;
    engine.renderSystem->modelArray = engine.modelComponents;
    engine.renderSystem->pointLightArray = engine.pointLightComponents;
    engine.renderSystem->LoadGrid();
    engine.renderSystem->LoadLight();

    TransformSystem transformSystem;
    engine.transformSystem = &transformSystem;
    engine.transformSystem->transformArray = engine.transformComponents;

    ScriptSystem scriptSystem;
    engine.scriptSystem = &scriptSystem;
    engine.scriptSystem->scriptArray = engine.scriptComponents;

    engine.scene->entityDataArray = engine.entityDataComponents;

    Entity light1 = engine.entityManager->CreateEntity();
    engine.pointLightComponents->AddComponent(light1, {
        glm::vec3(1.2f, 1.0f, 2.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    });
    engine.entityDataComponents->AddComponent(light1, {light1, "Light1"});

    engine.scene->entities.insert(light1);
    engine.renderSystem->entities.insert(light1);
    
    renderLoop(engine);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(engine.window);
    glfwTerminate();
    return 0;
}

void setupGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        exit(EXIT_FAILURE);
    }
}

GLFWwindow* createWindow(int width, int height, const char* title) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return nullptr;
    }
    glEnable(GL_DEPTH_TEST);
    return window;
}

void setupImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void processInput(float deltaTime) {
    ImGuiIO& io = ImGui::GetIO();

    if (ImGui::IsKeyDown(ImGuiKey_W)) // W key
        engine.camera->ProcessKeyboard(Camera::FORWARD, deltaTime);
    if (ImGui::IsKeyDown(ImGuiKey_S)) // S key
        engine.camera->ProcessKeyboard(Camera::BACKWARD, deltaTime);
    if (ImGui::IsKeyDown(ImGuiKey_A)) // A key
        engine.camera->ProcessKeyboard(Camera::LEFT, deltaTime);
    if (ImGui::IsKeyDown(ImGuiKey_D)) // D key
        engine.camera->ProcessKeyboard(Camera::RIGHT, deltaTime);
}

void processMouseScroll() {
    ImGuiIO& io = ImGui::GetIO();
    engine.camera->ProcessMouseScroll(io.MouseWheel);
}

void ShowSceneEntities() {
    std::vector<EntityDataComponent*> components = engine.scene->ListEntityData();
    std::vector<const char*> items;

    for (EntityDataComponent* c : components) {
        items.push_back(c->title.c_str());
    }

    ImVec2 listboxSize(300, 300);
    ImVec2 listboxPos(size_w - 300, 37);

    ImGui::SetNextWindowSize(listboxSize);
    ImGui::SetNextWindowPos(listboxPos);

    ImGui::Begin("Scene Entities");
    
    if (ImGui::ListBox("Entities", &selected_entity_idx, items.data(), items.size(), 4)) {
        selected_entity = components[selected_entity_idx]->entity;
    }
    ImGui::End();
    
}

void ComponentEditor() {

    ImVec2 componentEditorSize(300, 350);
    ImVec2 componentEditorPos(size_w - 300, 337);

    ImGui::SetNextWindowSize(componentEditorSize);
    ImGui::SetNextWindowPos(componentEditorPos);

    ImGui::Begin("Component Editor");

    if (selected_entity != 0) {
        // Entity Data Component
        EntityDataComponent* entityData = engine.entityDataComponents->GetComponent(selected_entity);

        if (entityData) {
            if (ImGui::CollapsingHeader("Data", ImGuiTreeNodeFlags_DefaultOpen)) {
                
            
                const int bufferSize = 256;
                char title[bufferSize];
                
                strncpy(title, entityData->title.c_str(), bufferSize);
                title[bufferSize - 1] = '\0';
            
                ImGui::BeginChild("EntityDataBox", ImVec2(0, 150), true);
                ImGui::Text("Title");
                if (ImGui::InputText("##Title", title, bufferSize)) {
                    entityData->title = std::string(title);
                }
                
                ImGui::EndChild();
            }
        }
        // Transform Component
        TransformComponent* transform = engine.transformComponents->GetComponent(selected_entity);

        if (transform) {
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

                

                float positionArray[3] = { transform->position.x, transform->position.y, transform->position.z };
                float rotationArray[3] = { transform->rotation.x, transform->rotation.y, transform->rotation.z };
                float scaleArray[3]    = { transform->scale.x, transform->scale.y, transform->scale.z };

                // Create a box-like appearance using a child window
                ImGui::BeginChild("TransformBox", ImVec2(0, 150), true);

                // Position
                ImGui::Text("Position");
                ImGui::InputFloat3("##Position", positionArray);
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    // Update the transform.position if the input values change
                    transform->position = glm::vec3(positionArray[0], positionArray[1], positionArray[2]);
                    engine.transformSystem->SetTransform();
                }

                // Rotation
                ImGui::Text("Rotation");
                ImGui::InputFloat3("##Rotation", rotationArray);
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    // Update the transform.rotation if the input values change
                    transform->rotation = glm::vec3(rotationArray[0], rotationArray[1], rotationArray[2]);
                    engine.transformSystem->SetTransform();
                }

                // Scale
                ImGui::Text("Scale");
                ImGui::InputFloat3("##Scale", scaleArray);
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    // Update the transform.scale if the input values change
                    transform->scale = glm::vec3(scaleArray[0], scaleArray[1], scaleArray[2]);
                    engine.transformSystem->SetTransform();
                }

                ImGui::EndChild();
            }
        }

        //PointLightComponent
        PointLightComponent* pointLight = engine.pointLightComponents->GetComponent(selected_entity);

        if (pointLight) {
            if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen)) {
                float positionArray[3] = { pointLight->position.x, pointLight->position.y, pointLight->position.z };
                float colorArray[3] = { pointLight->color[0], pointLight->color[1], pointLight->color[1] };

                ImGui::BeginChild("PointLightBox", ImVec2(0, 150), true);

                ImGui::Text("Position");
                ImGui::InputFloat3("##Position", positionArray);
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    // Update the transform.position if the input values change
                    pointLight->position = glm::vec3(positionArray[0], positionArray[1], positionArray[2]);
                }

                ImGui::Text("Color");
                ImGui::InputFloat3("##Color", colorArray);
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    // Update the transform.position if the input values change
                    pointLight->color = glm::vec3(colorArray[0], colorArray[1], colorArray[2]);
                }

                ImGui::EndChild();
            }
        }

        ScriptComponent* script = engine.scriptComponents->GetComponent(selected_entity);

        if (script) {
            if (ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen)) {
                const int bufferSize = 256;
                char filename[bufferSize];
                char classname[bufferSize];
                
                strncpy(filename, script->filename.c_str(), bufferSize);
                filename[bufferSize - 1] = '\0';
            
                ImGui::BeginChild("ScriptBox", ImVec2(0, 150), true);
                ImGui::Text("File Name");
                if (ImGui::InputText("##FileName", filename, bufferSize)) {
                    script->filename = std::string(filename);
                }
                
                ImGui::EndChild();
            }

        }

        const int bufferSize = 256;
        char scriptname[bufferSize];
        strncpy(scriptname, add_script_name.c_str(), bufferSize);
        scriptname[bufferSize - 1] = '\0';

        if (ImGui::InputText("##ScriptName", scriptname, bufferSize)) {
            add_script_name = std::string(scriptname);
        }
        if (ImGui::Button("Add Script")) {
            engine.scriptComponents->AddComponent(selected_entity, {
                selected_entity,
                add_script_name + ".lua"
            });
            GameData gamedata;

            std::string inputFilePath = "game/base/ScriptBase.lua";
            std::string outputFilePath = "game/scripts/" + add_script_name + ".lua";
            fs::copy_file(inputFilePath, outputFilePath);
        }
    }
    ImGui::End();
}

void SaveGame() {
    GameData gamedata;

    for (Entity entity : engine.transformSystem->entities) {
        gamedata.SaveEntity(entity, engine.transformComponents, engine.modelComponents);
    }

    std::vector<EntityDataComponent*> components = engine.scene->ListEntityData();
}

void renderLoop(Engine engine) {
    while (!glfwWindowShouldClose(engine.window)) {
        glfwPollEvents();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::StyleColorsLight();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(size_w, size_h));
        ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Import")) {
                    IGFD::FileDialogConfig config;
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".blend", config);
                }
                if (ImGui::MenuItem("Save")) {
                    SaveGame();
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndMenuBar();
        ImGui::End();

        ShowSceneEntities();
        ComponentEditor();

        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400), ImVec2(600, 400) )) {
            if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                std::string filename = std::filesystem::path(filePathName).filename();

                Entity openedFile = engine.entityManager->CreateEntity();

                engine.transformComponents->AddComponent(openedFile, {
                    glm::mat4(1.0f), 
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(1.0f, 1.0f, 1.0f),
                });
                std::string dst = "game/models/" + filename;
                std::filesystem::copy(filePathName, dst);
                Model myModel1(dst);
                engine.modelComponents->AddComponent(openedFile, {&myModel1, filename});
                engine.entityDataComponents->AddComponent(openedFile, {openedFile, "Entity" + std::to_string(openedFile)});

                engine.scene->entities.insert(openedFile);
                engine.renderSystem->entities.insert(openedFile);
                engine.transformSystem->entities.insert(openedFile);
            }
        
            ImGuiFileDialog::Instance()->Close();
        }

        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImVec2 windowSize(500, 380);
        ImVec2 windowPos(size_w / 2 - 250, 37);

        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos(windowPos);
        
        ImGui::Begin("Scene");
        ImVec2 avail = ImGui::GetContentRegionAvail();
        ImVec2 size(avail.x, avail.y); // Fixed size for the framebuffer

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (ImGui::IsMouseClicked(0) && // Left mouse button clicked
            ImGui::IsWindowHovered()) { // Mouse is inside the scene window
            toggleCamera(engine.window, true); // Activate the camera
        }

        if (cameraActive && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            toggleCamera(engine.window, false); // Deactivate the camera
        }

        if (cameraActive) {
            double mouseX, mouseY;
            glfwGetCursorPos(engine.window, &mouseX, &mouseY);
        
            float xoffset = mouseX - lastMouseX;
            float yoffset = lastMouseY - mouseY; // Reversed since y-coordinates go from bottom to top
        
            lastMouseX = mouseX;
            lastMouseY = mouseY;
        
            processInput(deltaTime);
            engine.camera->ProcessMouseMovement(xoffset, yoffset);
            processMouseScroll();
        }
        
        
        // Bind framebuffer to render scene
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, 500, 380);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        engine.transformSystem->Update(deltaTime);
        engine.renderSystem->camera = engine.camera;
        engine.renderSystem->size = glm::vec2(size.x, size.y);
        engine.renderSystem->gridShader = engine.gridShader;
        engine.renderSystem->DrawGrid(deltaTime);
        engine.renderSystem->lightShader = engine.lightShader;
        engine.renderSystem->DrawLight(deltaTime);
        engine.renderSystem->shader = engine.defaultShader;
        engine.renderSystem->Update(deltaTime);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the framebuffer

        // Render framebuffer texture inside ImGui container
        ImGui::Image((ImTextureID)(intptr_t)fboTexture, size, ImVec2(0, 1), ImVec2(1, 0));
        
        ImGui::End();
        
        ImGui::Render();

        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(engine.window);
    }
}


