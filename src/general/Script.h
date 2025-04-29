#ifndef SCRIPT_H
#define SCRIPT_H

#include "../structs.cpp"
#include "Engine.cpp"
#include <sol/sol.hpp>
#include <memory>

class Script {
public:
    Script(Entity entity, const std::string& filename, Engine* engine);
    ~Script();

    void Load();
    void Update(float deltaTime);

    Engine* m_engine;
    
    // Allow moving but prevent copying
    Script(Script&&) = default;
    Script& operator=(Script&&) = default;
    Script(const Script&) = delete;
    Script& operator=(const Script&) = delete;

private:
    uint32_t m_entity;
    std::string m_filename;
    
    sol::environment m_env;
    sol::function m_updateFunc;
    bool m_hasUpdate = false;
};

#endif