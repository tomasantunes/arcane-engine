#ifndef SCRIPT_CPP
#define SCRIPT_CPP

#include "../structs.cpp"
#include "Engine.cpp"

class Script {
    public:
        Entity entity;
        Engine* engine;
        virtual void Load() {}
        virtual void Update(float deltaTime) {}
};

#endif