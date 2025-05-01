#include "ScriptSystem.h"
#include "Script.h"
#include "ScriptComponent.cpp"

ScriptSystem::ScriptSystem() {
    // Only initialize Lua here
    m_lua.open_libraries(
        sol::lib::base,
        sol::lib::package,
        sol::lib::math,
        sol::lib::string,
        sol::lib::table
    );
}

void ScriptSystem::Initialize(Engine* engine) {
    m_engine = engine;
    // Any other engine-dependent initialization
}

void ScriptSystem::Update(float deltaTime) {
    for (auto& script : m_scripts) {
        script->Update(deltaTime);
    }
}

void ScriptSystem::ReloadAllScripts() {
    m_scripts.clear();
    
    if (!m_engine || !m_engine->scriptComponents) return;
    
    // Assuming your component array has a way to iterate
    for (size_t i = 0; i < m_engine->scriptComponents->size(); ++i) {
        auto& comp = (*m_engine->scriptComponents)[i];
        if (comp.filename.empty()) continue;
        
        m_scripts.emplace_back(
            std::make_unique<Script>(m_engine, comp.entity, comp.filename)
        );
        m_scripts.back()->Load();
    }
}