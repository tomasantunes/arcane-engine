#ifndef SCRIPTHELPERFUNCTIONS_CPP
#define SCRIPTHELPERFUNCTIONS_CPP

#include <sol/sol.hpp>
#include <glm/glm.hpp>

auto mat4_to_table = [](lua_State* L, const glm::mat4& m) {
    sol::table t = sol::state_view(L).create_table();
    for (int i = 0; i < 4; ++i) {
        t[i+1] = sol::state_view(L).create_table();
        for (int j = 0; j < 4; ++j) {
            t[i+1][j+1] = m[i][j];
        }
    }
    return t;
};

auto table_to_mat4 = [](sol::table t) {
    glm::mat4 m;
    for (int i = 0; i < 4; ++i) {
        sol::table row = t[i+1];
        for (int j = 0; j < 4; ++j) {
            m[i][j] = row[j+1];
        }
    }
    return m;
};

#endif