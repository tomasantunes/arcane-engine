#include "Script.h"

Script::Script(sol::state* e_lua, Engine* engine, Entity entity, const std::string& filename)
    : m_lua(e_lua), m_engine(engine), m_entity(entity), m_filename(filename) {
}

void Script::Load() {
    try {
        m_env = sol::environment(*m_lua, sol::create, m_lua->globals());
        ExposeEngineAPI();
        
        // Load the script file into the environment
        auto result = m_lua->script_file("scripts/" + m_filename, m_env);
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << "Failed to load script: " << err.what() << std::endl;
            return;
        }

        // Check for load function (must be a function if it exists)
        sol::object loadObj = m_env["load"];
        if (loadObj.is<sol::function>()) {
            sol::function loadFunc = loadObj;
            auto loadResult = loadFunc(m_entity);
            if (!loadResult.valid()) {
                sol::error err = loadResult;
                std::cerr << "load function error: " << err.what() << std::endl;
            }
        }

        // Check for update function
        sol::object updateObj = m_env["update"];
        if (updateObj.is<sol::function>()) {
            m_updateFunc = updateObj;
            m_hasUpdate = true;
            std::cout << "Script has update function." << std::endl;
        } else {
            m_hasUpdate = false;
            std::cout << "Script doesn't have update function." << std::endl;
        }
        
    } catch (const sol::error& e) {
        std::cerr << "LUA ERROR: " << e.what() << std::endl;
    }
}

void Script::Update(float deltaTime) {
    if (!m_hasUpdate) {
        return;
    }
    
    try {
        auto result = m_updateFunc(m_entity, deltaTime);
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << "update function error: " << err.what() << std::endl;
        }
    } catch (const sol::error& e) {
        std::cerr << "LUA ERROR: " << e.what() << std::endl;
    }
}

void Script::ExposeEngineAPI() {
    m_env["print"] = [](const std::string& msg) {
        std::cout << "[LUA] " << msg << std::endl;
    };

    m_lua->new_usertype<TransformComponent>("TransformComponent",
        sol::constructors<TransformComponent()>(),
        
        // Properties
        "transform", sol::property(
            [](TransformComponent& tc, sol::this_state L) { 
                return mat4_to_table(L, tc.transform); 
            },
            [](TransformComponent& tc, sol::table m) { 
                tc.transform = table_to_mat4(m);
            }
        ),
        "position", &TransformComponent::position,
        "rotation", &TransformComponent::rotation,
        "scale", &TransformComponent::scale
    );

    m_lua->new_usertype<TransformSystem>("TransformSystem",
        "updateTransforms", &TransformSystem::SetTransform
    );

    m_lua->set_function("getTransform", [this](Entity entity) -> sol::reference {
        TransformComponent* tc = m_engine->transformComponents->GetComponent(entity);
        if (!tc) {
            return sol::lua_nil;
        }
        return sol::make_reference(m_lua->lua_state(), std::ref(*tc));
    });

    m_lua->set_function("setTransform", [this](Entity entity, sol::object transformObj) {
        TransformComponent* tc = m_engine->transformComponents->GetComponent(entity);
        if (!tc) {
            luaL_error(m_lua->lua_state(), "Entity has no TransformComponent");
            return;
        }
    
        if (transformObj.is<TransformComponent>()) {
            *tc = transformObj.as<TransformComponent>();
        } else {
            luaL_error(m_lua->lua_state(), "Expected TransformComponent");
        }
    });
}