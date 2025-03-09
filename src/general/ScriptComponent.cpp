#ifndef SCRIPTCOMPONENT_CPP
#define SCRIPTCOMPONENT_CPP

#include <string>
#include "../structs.cpp"

struct ScriptComponent {
    Entity entity;
    std::string filename;
    std::string classname;
};

#endif