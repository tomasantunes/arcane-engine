#ifndef SCRIPT_H
#define SCRIPT_H

#include "../structs.cpp"
#include "Engine.cpp"
#include <sol/sol.hpp>
#include <memory>

class Script {
public:
    Script(uint32_t entity, const std::string& filename, EntityManager* manager);
    ~Script();

    void Load();
    void Update(float deltaTime);
    
    // Allow moving but prevent copying
    Script(Script&&) = default;
    Script& operator=(Script&&) = default;
    Script(const Script&) = delete;
    Script& operator=(const Script&) = delete;

private:
    uint32_t m_entity;
    std::string m_filename;
    EntityManager* m_manager;
    
    sol::environment m_env;
    sol::function m_updateFunc;
    bool m_hasUpdate = false;
};

#endif