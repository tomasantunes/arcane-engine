#pragma once
#include <vector>
#include <memory>
#include <sol/sol.hpp>

class Engine;

class ScriptSystem {
public:
    ScriptSystem(Engine* engine);
    
    void Update(float deltaTime);
    void ReloadAllScripts();
    
    sol::state& GetLuaState() { return m_lua; }

private:
    Engine* m_engine;
    sol::state m_lua;
    std::vector<std::unique_ptr<Script>> m_scripts;
};