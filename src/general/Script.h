#pragma once
#include <sol/sol.hpp>
#include <string>

// Forward declarations
class ScriptSystem;  // Only need ScriptSystem, not full Engine

class Script {
public:
    Script(ScriptSystem* system, uint32_t entity, const std::string& filename);
    
    void Load();
    void Update(float deltaTime);
    
    // Prevent copying
    Script(const Script&) = delete;
    Script& operator=(const Script&) = delete;
    
    // Allow moving
    Script(Script&&) = default;
    Script& operator=(Script&&) = default;

private:
    ScriptSystem* m_system;
    uint32_t m_entity;
    std::string m_filename;
    
    sol::environment m_env;
    sol::function m_updateFunc;
    bool m_hasUpdate = false;
    
    void ExposeEngineAPI();
};