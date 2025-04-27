#include "ScriptSystem.h"
#include "EntityManager.cpp"
#include "Script.h"
#include <iostream>

ScriptSystem::ScriptSystem(EntityManager* manager) 
    : m_manager(manager) {
    // Initialize Lua state with common libraries
    m_lua.open_libraries(
        sol::lib::base,
        sol::lib::package,
        sol::lib::math,
        sol::lib::string,
        sol::lib::table
    );
    
    // Expose basic engine functionality to Lua
    m_lua["print"] = [](const std::string& msg) {
        std::cout << "[LUA] " << msg << std::endl;
    };
}

ScriptSystem::~ScriptSystem() {
    m_scripts.clear();
}

void ScriptSystem::AddScript(uint32_t entity, const std::string& filename) {
    m_scripts.emplace_back(std::make_unique<Script>(entity, filename, m_manager));
    m_scripts.back()->Load();
}

void ScriptSystem::Update(float deltaTime) {
    for (auto& script : m_scripts) {
        script->Update(deltaTime);
    }
}

void ScriptSystem::ReloadAllScripts() {
    for (auto& script : m_scripts) {
        script->Load();
    }
}