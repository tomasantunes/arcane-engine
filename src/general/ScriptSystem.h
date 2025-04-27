#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H

#include <vector>
#include <memory>
#include <sol/sol.hpp>

class ScriptSystem {
public:
    ScriptSystem(EntityManager* manager);
    ~ScriptSystem();

    void Update(float deltaTime);
    void AddScript(uint32_t entity, const std::string& filename);
    void ReloadAllScripts();

    // Access to Lua state for exposing C++ functions
    sol::state& GetLuaState() { return m_lua; }

private:
    sol::state m_lua;
    EntityManager* m_manager;
    std::vector<std::unique_ptr<Script>> m_scripts;
};

#endif