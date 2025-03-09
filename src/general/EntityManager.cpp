#ifndef ENTITYMANAGER_CPP
#define ENTITYMANAGER_CPP

#include <iostream>
#include <vector>
#include <algorithm>
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

        void LoadEntity(Entity entity) {
            // Use std::remove to move all instances of the value to the end
            auto new_end = std::remove(availableEntities.begin(), availableEntities.end(), entity);

            // Erase the "removed" elements from the vector
            availableEntities.erase(new_end, availableEntities.end());
        }

        void DestroyEntity(Entity entity) {
            availableEntities.push_back(entity);
        }
};

#endif