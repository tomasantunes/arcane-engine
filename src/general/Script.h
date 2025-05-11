#pragma once
#include <sol/sol.hpp>
#include <string>
#include "../structs.cpp"
#include "ScriptSystem.h"
#include "../graphics/TransformSystem.cpp"

class Script {
public:
    Script(sol::state* e_lua, Engine* engine,  Entity entity, const std::string& filename);
    
    void Load();
    void Update(float deltaTime);
    
    // Prevent copying
    Script(const Script&) = delete;
    Script& operator=(const Script&) = delete;
    
    // Allow moving
    Script(Script&&) = default;
    Script& operator=(Script&&) = default;

private:
    sol::state* m_lua;
    Engine* m_engine;
    Entity m_entity;
    std::string m_filename;
    
    sol::environment m_env;
    sol::function m_updateFunc;
    bool m_hasUpdate = false;
    
    void ExposeEngineAPI();
};