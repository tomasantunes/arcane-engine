#ifndef COMPONENTMANAGER_CPP
#define COMPONENTMANAGER_CPP

#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>
#include "../structs.cpp"

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void RemoveEntity(Entity entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray {
private:
    std::unordered_map<Entity, T> componentMap;

public:
    void AddComponent(Entity entity, T component) {
        componentMap[entity] = component;
    }

    void RemoveEntity(Entity entity) override {
        componentMap.erase(entity);
    }

    T* GetComponent(Entity entity) {
        if (componentMap.find(entity) != componentMap.end())
            return &componentMap[entity];
        return nullptr;
    }
};

#endif