#ifndef SCRIPT_CPP
#define SCRIPT_CPP

#include "../structs.cpp"

class Script {
    public:
        Entity entity;
        virtual void Load() {}
        virtual void Update(float deltaTime) {}
};

#endif