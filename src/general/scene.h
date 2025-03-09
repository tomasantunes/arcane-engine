#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "ComponentManager.cpp"
#include "EntityDataComponent.cpp"
#include "System.cpp"

class Scene : public System {
    public:
        Scene();
        ComponentArray<EntityDataComponent>* entityDataArray;

        std::vector<EntityDataComponent*> ListEntityData();

        void Update(float dt) override;
    private:

    };

#endif