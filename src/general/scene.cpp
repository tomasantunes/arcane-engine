#include "scene.h"

Scene::Scene() {
    
}

std::vector<EntityDataComponent*> Scene::ListEntityData() {
    std::vector<EntityDataComponent*> components;
    for (Entity entity : entities) {
        EntityDataComponent* e = entityDataArray->GetComponent(entity);
        components.push_back(e);
    }
    return components;
}

void Scene::Update(float dt) {
    
}