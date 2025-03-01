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
#include "structs.cpp"
#include "constants.cpp"
#include "general/EntityManager.cpp"
#include "general/ComponentManager.cpp"
#include "general/scene.h"
#include "graphics/model.h"
#include "graphics/shader.h"
#include "graphics/GraphicsComponents.cpp"
#include "graphics/RenderSystem.cpp"
#include "graphics/Camera.cpp"

int size_w = 1280;
int size_h = 768;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool cameraActive = false; // Tracks whether the camera is active
bool cursorLocked = false; // Tracks whether the cursor is locked
double lastMouseX = 0.0, lastMouseY = 0.0; // Stores the last mouse position
Camera camera;

void setupGLFW();
GLFWwindow* createWindow(int width, int height, const char* title);
void setupImGui(GLFWwindow* window);
void renderLoop(GLFWwindow* window, RenderSystem renderSystem, Shader* defaultShader, Shader* gridShader, EntityManager* entityManager, ComponentArray<ModelComponent>* modelComponents, ComponentArray<TransformComponent>* transformComponents);

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

    setupImGui(window);

    setupFramebuffer(512, 512);

    Shader defaultShader("shaders/default-vshader.glsl", "shaders/default-fshader.glsl");
    Shader gridShader("shaders/grid-vshader.glsl", "shaders/grid-fshader.glsl");

    EntityManager entityManager;
    ComponentArray<TransformComponent> transformComponents;
    ComponentArray<ModelComponent> modelComponents;

    RenderSystem renderSystem;
    renderSystem.transformArray = &transformComponents;
    renderSystem.modelArray = &modelComponents;
    renderSystem.LoadGrid();
    
    renderLoop(window, renderSystem, &defaultShader, &gridShader, &entityManager, &modelComponents, &transformComponents);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
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
        camera.ProcessKeyboard(Camera::FORWARD, deltaTime);
    if (ImGui::IsKeyDown(ImGuiKey_S)) // S key
        camera.ProcessKeyboard(Camera::BACKWARD, deltaTime);
    if (ImGui::IsKeyDown(ImGuiKey_A)) // A key
        camera.ProcessKeyboard(Camera::LEFT, deltaTime);
    if (ImGui::IsKeyDown(ImGuiKey_D)) // D key
        camera.ProcessKeyboard(Camera::RIGHT, deltaTime);
}

void processMouseScroll() {
    ImGuiIO& io = ImGui::GetIO();
    camera.ProcessMouseScroll(io.MouseWheel);
}

void renderLoop(GLFWwindow* window, RenderSystem renderSystem, Shader* defaultShader, Shader* gridShader, EntityManager* entityManager, ComponentArray<ModelComponent>* modelComponents, ComponentArray<TransformComponent>* transformComponents) {
    while (!glfwWindowShouldClose(window)) {
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
                ImGui::EndMenu();
            }
        }
        ImGui::EndMenuBar();
        ImGui::End();

        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400), ImVec2(600, 400) )) {
            if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                Entity openedFile = entityManager->CreateEntity();

                transformComponents->AddComponent(openedFile, {
                    glm::mat4(1.0f), 
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(1.0f, 1.0f, 1.0f),
                });
                Model myModel1(filePathName);
                modelComponents->AddComponent(openedFile, {&myModel1});

                renderSystem.entities.insert(openedFile);
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
            toggleCamera(window, true); // Activate the camera
        }

        if (cameraActive && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            toggleCamera(window, false); // Deactivate the camera
        }

        if (cameraActive) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
        
            float xoffset = mouseX - lastMouseX;
            float yoffset = lastMouseY - mouseY; // Reversed since y-coordinates go from bottom to top
        
            lastMouseX = mouseX;
            lastMouseY = mouseY;
        
            processInput(deltaTime);
            camera.ProcessMouseMovement(xoffset, yoffset);
            processMouseScroll();
        }
        
        
        // Bind framebuffer to render scene
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, 530, 530);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderSystem.camera = &camera;
        renderSystem.size = glm::vec2(size.x, size.y);
        renderSystem.gridShader = gridShader;
        renderSystem.DrawGrid(deltaTime);
        renderSystem.shader = defaultShader;
        renderSystem.Update(deltaTime);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the framebuffer

        // Render framebuffer texture inside ImGui container
        ImGui::Image((ImTextureID)(intptr_t)fboTexture, size, ImVec2(0, 1), ImVec2(1, 0));
        
        ImGui::End();
        
        ImGui::Render();

        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}


