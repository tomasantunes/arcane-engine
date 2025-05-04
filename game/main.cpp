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
#include "src/structs.cpp"
#include "src/constants.cpp"
#include "src/general/Engine.cpp"
#include "src/general/EntityManager.cpp"
#include "src/general/ComponentManager.cpp"
#include "src/general/EntityDataComponent.cpp"
#include "src/general/ScriptComponent.cpp"
#include "src/general/ScriptSystem.h"
#include "src/general/GameData.cpp"
#include "src/general/scene.h"
#include "src/graphics/model.h"
#include "src/graphics/shader.h"
#include "src/graphics/GraphicsComponents.cpp"
#include "src/graphics/RenderSystem.cpp"
#include "src/graphics/TransformSystem.cpp"
#include "src/graphics/Camera.cpp"
#include <sol/sol.hpp>

int size_w = 1280;
int size_h = 768;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool cameraActive = false; // Tracks whether the camera is active
bool cursorLocked = false; // Tracks whether the cursor is locked
double lastMouseX = 0.0, lastMouseY = 0.0; // Stores the last mouse position
static int selected_entity_idx = 0;
Entity selected_entity = 0;
Engine engine;
sol::state m_lua;

void setupGLFW();
GLFWwindow* createWindow(int width, int height, const char* title);
void setupImGui(GLFWwindow* window);
void renderLoop(Engine engine);

GLuint fbo, fboTexture, rbo;

int main() {
    setupGLFW();
    GLFWwindow* window = createWindow(size_w, size_h, "Game");
    if (!window) return -1;
    engine.window = window;

    setupImGui(engine.window);

    m_lua.open_libraries(
        sol::lib::base,
        sol::lib::package,
        sol::lib::math,
        sol::lib::string,
        sol::lib::table
    );

    engine.mode = "game";
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
    ComponentArray<ScriptComponent> scriptComponents;
    ComponentArray<PointLightComponent> pointLightComponents;
    engine.entityManager = &entityManager;
    engine.transformComponents = &transformComponents;
    engine.modelComponents = &modelComponents;
    engine.entityDataComponents = &entityDataComponents;
    engine.scriptComponents = &scriptComponents;
    engine.pointLightComponents = &pointLightComponents;

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

    ScriptSystem scriptSystem(&m_lua);
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

    GameData gamedata;

    std::vector<Entity> loadedEntities = gamedata.LoadEntities();

    for (Entity entity : loadedEntities) {
        entityManager.LoadEntity(entity);
        std::string filename = gamedata.LoadEntityModel(entity);
        TransformComponent transform = gamedata.LoadEntityTransform(entity);

        engine.transformComponents->AddComponent(entity, transform);
        Model myModel1("models/" + filename);
        engine.modelComponents->AddComponent(entity, {&myModel1, filename});
        engine.entityDataComponents->AddComponent(entity, {entity, "Entity" + std::to_string(entity)});

        engine.scene->entities.insert(entity);
        engine.renderSystem->entities.insert(entity);
        engine.transformSystem->entities.insert(entity);
        engine.scriptSystem->entities.insert(entity);

        std::string script_filename = gamedata.LoadEntityScript(entity);
        engine.scriptComponents->AddComponent(entity, {entity, script_filename});
    }

    engine.scriptSystem->ReloadAllScripts();
    
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

void renderLoop(Engine engine) {
    while (!glfwWindowShouldClose(engine.window)) {
        glfwPollEvents();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::StyleColorsLight();
        ImGui::NewFrame();

        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        
        
        // Bind framebuffer to render scene
        glViewport(0, 0, size_w, size_h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        engine.transformSystem->Update(deltaTime);
        engine.renderSystem->camera = engine.camera;
        engine.renderSystem->size = glm::vec2(size_w, size_h);
        engine.renderSystem->gridShader = engine.gridShader;
        engine.renderSystem->DrawGrid(deltaTime);
        engine.renderSystem->lightShader = engine.lightShader;
        engine.renderSystem->DrawLight(deltaTime);
        engine.renderSystem->shader = engine.defaultShader;
        engine.renderSystem->Update(deltaTime);
        
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(engine.window);
    }
}


