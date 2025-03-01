#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../lib/ImGuiFileDialog/ImGuiFileDialog.h"
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
Camera camera;
void setupGLFW();
GLFWwindow* createWindow(int width, int height, const char* title);
void setupImGui(GLFWwindow* window);
void renderLoop(GLFWwindow* window, Shader shader, RenderSystem renderSystem, EntityManager* entityManager, ComponentArray<ModelComponent>* modelComponents, ComponentArray<TransformComponent>* transformComponents);

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


int main() {
    setupGLFW();
    GLFWwindow* window = createWindow(size_w, size_h, "ArcaneEngine");
    if (!window) return -1;

    setupImGui(window);

    Shader myShader("shaders/default-vshader.glsl", "shaders/default-fshader.glsl");

    setupFramebuffer(512, 512);

    EntityManager entityManager;
    ComponentArray<TransformComponent> transformComponents;
    ComponentArray<ModelComponent> modelComponents;

    RenderSystem renderSystem;
    renderSystem.transformArray = &transformComponents;
    renderSystem.modelArray = &modelComponents;
    
    renderLoop(window, myShader, renderSystem, &entityManager, &modelComponents, &transformComponents);

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

void processMouseInput(const ImVec2& windowPos, const ImVec2& windowSize) {
    ImGuiIO& io = ImGui::GetIO();

    // Check if the mouse is inside the ImGui window
    if (io.MousePos.x >= windowPos.x && io.MousePos.x <= windowPos.x + windowSize.x &&
        io.MousePos.y >= windowPos.y && io.MousePos.y <= windowPos.y + windowSize.y) {
        static bool firstMouse = true;
        static float lastX = windowPos.x + windowSize.x / 2.0f;
        static float lastY = windowPos.y + windowSize.y / 2.0f;

        if (firstMouse) {
            lastX = io.MousePos.x;
            lastY = io.MousePos.y;
            firstMouse = false;
        }

        float xoffset = io.MousePos.x - lastX;
        float yoffset = lastY - io.MousePos.y; // Reversed since y-coordinates go from bottom to top

        lastX = io.MousePos.x;
        lastY = io.MousePos.y;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void processMouseScroll() {
    ImGuiIO& io = ImGui::GetIO();
    camera.ProcessMouseScroll(io.MouseWheel);
}

void renderLoop(GLFWwindow* window, Shader shader, RenderSystem renderSystem, EntityManager* entityManager, ComponentArray<ModelComponent>* modelComponents, ComponentArray<TransformComponent>* transformComponents) {
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
                    config.path = ".";
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".blend", config);
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndMenuBar();

        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
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

        processInput(deltaTime);
        processMouseInput(windowPos, windowSize);
        processMouseScroll();

        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);    // Light position
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);  // White light
        glm::vec3 objectColor(1.0f, 0.5f, 0.2f); // Orange object
        
        // Bind framebuffer to render scene
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, 512, 512);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader.Program);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)size.x / (float)size.y, 0.1f, 100.0f);
        
        GLint modelLoc = glGetUniformLocation(shader.Program, "model");
        GLint viewLoc = glGetUniformLocation(shader.Program, "view");
        GLint projLoc = glGetUniformLocation(shader.Program, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        shader.setVec3("lightPos", lightPos);
        shader.setVec3("lightColor", lightColor);
        shader.setVec3("objectColor", objectColor);

        
        
        renderSystem.shader = &shader;
        renderSystem.Update(1.0f);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the framebuffer

        // Render framebuffer texture inside ImGui container
        ImGui::Image((ImTextureID)(intptr_t)fboTexture, size, ImVec2(0, 1), ImVec2(1, 0));
        
        ImGui::End();
        ImGui::End();
        
        ImGui::Render();

        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}


