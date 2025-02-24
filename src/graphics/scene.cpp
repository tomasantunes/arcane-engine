#include "scene.h"

Scene::Scene() {
    
}

void Scene::AddModel(Model model) {
    models.push_back(model);
}