#include "Script.h"
#include "ScriptSystem.h"  // Only include ScriptSystem header

// Forward declarations for any engine types you need to expose
class TransformComponent;  // Example component

Script::Script(ScriptSystem* system, uint32_t entity, const std::string& filename)
    : m_system(system), m_entity(entity), m_filename(filename) {
}

void Script::Load() {
    auto& lua = m_system->GetLuaState();
    
    try {
        m_env = sol::environment(lua, sol::create, lua.globals());
        ExposeEngineAPI();
        lua.script_file(m_filename, m_env);
        
        if (auto loadFunc = m_env["load"]; loadFunc) {
            loadFunc(m_entity);
        }
        
        m_updateFunc = m_env["update"];
        m_hasUpdate = m_updateFunc.valid();
        
    } catch (const sol::error& e) {
        // Error handling...
    }
}

void Script::Update(float deltaTime) {
    if (m_hasUpdate) {
        try {
            m_updateFunc(m_entity, deltaTime);
        } catch (const sol::error& e) {
            // Error handling...
        }
    }
}

void Script::ExposeEngineAPI() {
    // Or expose basic functions directly
    m_env["log"] = [](const std::string& msg) {
        std::cout << "[LUA] " << msg << std::endl;
    };
}