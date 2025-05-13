#include "ScriptSystem.h"

ScriptSystem::ScriptSystem(sol::state* e_lua, Engine* e_engine) : m_lua(e_lua), m_engine(e_engine) {
    
}

void ScriptSystem::Update(float deltaTime) {
    for (auto& [entity, script] : m_scripts) {
        if (script) {
            script->Update(deltaTime);
        }
    }
}

void ScriptSystem::AddScript(Entity entity, const std::string& filename) {
    // Create and store the script
    m_scripts[entity] = std::make_shared<Script>(m_lua, m_engine, entity, filename);
    entities.insert(entity);
}

void ScriptSystem::ReloadAllScripts() {
    for (auto& [entity, script] : m_scripts) {
        if (script) {
            script->Load();
        }
    }
}