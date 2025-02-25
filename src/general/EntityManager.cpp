#ifndef ENTITYMANAGER_CPP
#define ENTITYMANAGER_CPP

#include <iostream>
#include <vector>
#include "../constants.cpp"
#include "../structs.cpp"

class EntityManager {
    private:
        std::vector<Entity> availableEntities;
        Entity nextID = 0;

    public:
        EntityManager() {
            for (Entity i = 0; i < MAX_ENTITIES; i++) {
                availableEntities.push_back(i);
            }
        }

        Entity CreateEntity() {
            if (availableEntities.empty()) {
                throw std::runtime_error("Max entities reached.");
            }

            Entity id = availableEntities.back();
            availableEntities.pop_back();
            return id;
        }

        void DestroyEntity(Entity entity) {
            availableEntities.push_back(entity);
        }
};

#endif