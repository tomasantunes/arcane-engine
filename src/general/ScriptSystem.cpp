#include "ScriptSystem.h"
#include "Script.h"
#include "ScriptComponent.cpp"

ScriptSystem::ScriptSystem(sol::state* e_lua) : m_lua(e_lua) {
    
}

void ScriptSystem::Update(float deltaTime) {
    for (Entity entity : entities) {
        ScriptComponent* s = scriptArray->GetComponent(entity);
        Script script(m_lua, entity, s->filename);
        script.Update(deltaTime);
    }
}

void ScriptSystem::ReloadAllScripts() {
    std::cout << "ReloadAllScripts" << std::endl;
    for (Entity entity : entities) {
        std::cout << "For loop entered" << std::endl;
        ScriptComponent* s = scriptArray->GetComponent(entity);
        Script script(m_lua, entity, s->filename);
        script.Load();
    }
}