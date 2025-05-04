#include "Script.h"
#include "ScriptSystem.h"

Script::Script(sol::state* e_lua, Entity entity, const std::string& filename)
    : m_lua(e_lua), m_entity(entity), m_filename(filename) {
}

void Script::Load() {
    try {
        m_env = sol::environment(*m_lua, sol::create, m_lua->globals());
        ExposeEngineAPI();
        m_lua->script_file("scripts/" + m_filename, m_env);
        
        if (auto loadFunc = m_env["load"]; loadFunc) {
            loadFunc(m_entity);
        }
        
        m_updateFunc = m_env["update"];
        m_hasUpdate = m_updateFunc.valid();
        
    } catch (const sol::error& e) {
        std::cout << "LUA ERROR" << std::endl;
    }
}

void Script::Update(float deltaTime) {
    if (m_hasUpdate) {
        try {
            m_updateFunc(m_entity, deltaTime);
        } catch (const sol::error& e) {
            std::cout << "LUA ERROR" << std::endl;
        }
    }
}

void Script::ExposeEngineAPI() {
    m_env["print"] = [](const std::string& msg) {
        std::cout << "[LUA] " << msg << std::endl;
    };
}