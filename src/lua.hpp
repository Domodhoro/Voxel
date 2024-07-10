#ifndef LUA_HPP
#define LUA_HPP

lua_State *lua_init() {
    lua_State *L = luaL_newstate();

    if (!L) {
        std::cerr << "Falha ao criar estado lua." << std::endl;

        return nullptr;
    }

    luaL_openlibs(L);

    if (luaL_dofile(L, "./shaders/shaders.lua") != LUA_OK) {
        std::cerr << "Falha ao ler arquivo lua: " << lua_tostring(L, -1) << std::endl;

        return nullptr;
    }

    return L;
}

double luaToNumber(lua_State *L, const std::string &ID) {
    lua_getglobal(L, ID.c_str());

    double number = lua_tonumber(L, -1);

    lua_pop(L, 1);

    return number;
}

std::string luaToString(lua_State *L, const std::string &ID) {
    lua_getglobal(L, ID.c_str());

    std::string string = lua_tostring(L, -1);

    lua_pop(L, 1);

    return string;
}

#endif