#include "Script.h"
#include "EntityManager.cpp"
#include <iostream>

Script::Script(uint32_t entity, const std::string& filename, Engine* engine)
    : m_entity(entity), m_filename(filename), m_engine(engine) {
}

void Script::Load() {
    sol::state& lua = m_engine->scriptSystem.GetLuaState();
    
    try {
        // Create environment for this script
        m_env = sol::environment(lua, sol::create, lua.globals());
        
        // Load the script file
        lua.script_file(m_filename, m_env);
        
        // Call load function if it exists
        sol::function loadFunc = m_env["load"];
        if (loadFunc) {
            loadFunc(m_entity);  // Pass entity ID to load function
        }
        
        // Store update function if it exists
        m_updateFunc = m_env["update"];
        m_hasUpdate = m_updateFunc.valid();
    } catch (const sol::error& e) {
        std::cerr << "Script error in " << m_filename << ": " << e.what() << std::endl;
    }
}

void Script::Update(float deltaTime) {
    if (m_hasUpdate) {
        try {
            m_updateFunc(m_entity, deltaTime);  // Pass entity ID and delta time
        } catch (const sol::error& e) {
            std::cerr << "Update error in " << m_filename << ": " << e.what() << std::endl;
        }
    }
}