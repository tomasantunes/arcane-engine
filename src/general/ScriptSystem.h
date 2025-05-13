#pragma once
#include <vector>
#include <memory>
#include <sol/sol.hpp>
#include "Engine.cpp"
#include "System.cpp"
#include "ComponentManager.cpp"
#include "ScriptComponent.cpp"
#include "Script.h"

class ScriptSystem: public System {
public:
    ScriptSystem(sol::state* e_lua, Engine* e_engine);

    ComponentArray<ScriptComponent>* scriptArray;
    std::unordered_map<Entity, std::shared_ptr<Script>> m_scripts;
    
    void Update(float deltaTime);
    void ReloadAllScripts();
    void AddScript(Entity entity, const std::string& filename);

private:
    sol::state* m_lua;
    Engine* m_engine;

};