#pragma once
#include <vector>
#include <memory>
#include <sol/sol.hpp>
#include "System.cpp"
#include "ComponentManager.cpp"
#include "ScriptComponent.cpp"

class ScriptSystem: public System {
public:
    ScriptSystem(sol::state* e_lua);

    ComponentArray<ScriptComponent>* scriptArray;
    
    void Update(float deltaTime);
    void ReloadAllScripts();

private:
    sol::state* m_lua;

};