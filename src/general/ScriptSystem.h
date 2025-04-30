#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H

#include <vector>
#include <memory>
#include <sol/sol.hpp>
#include "../structs.cpp"
#include "System.cpp"
#include "ComponentManager.cpp"
#include "ScriptComponent.cpp"
#include "Script.h"

class ScriptSystem: public System {
public:
    ScriptSystem();
    ~ScriptSystem();

    void Update(float deltaTime);
    void AddScript(Entity entity, const std::string& filename);
    void ReloadAllScripts();

    // Access to Lua state for exposing C++ functions
    sol::state& GetLuaState() { return m_lua; }
    ComponentArray<ScriptComponent>* scriptArray;

private:
    sol::state m_lua;
    std::vector<std::unique_ptr<Script>> m_scripts;
};

#endif