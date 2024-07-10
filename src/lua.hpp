#ifndef LUA_HPP
#define LUA_HPP

class Lua {
public:
    bool init() {
        L = luaL_newstate();

        if (!L) {
            std::cerr << "Falha ao criar estado lua." << std::endl;

            return false;
        }

        luaL_openlibs(L);

        return true;
    }

    bool load() {
        if (luaL_dofile(L, "./luaFiles/shaders.lua") != LUA_OK) {
            std::cerr << "Falha ao ler arquivo lua: " << lua_tostring(L, -1) << std::endl;

            return false;
        }

        if (luaL_dofile(L, "./luaFiles/config.lua") != LUA_OK) {
            std::cerr << "Falha ao ler arquivo lua: " << lua_tostring(L, -1) << std::endl;

            return false;
        }

        return true;
    }

    double toNumber(const std::string &tableName, const std::string &variableName) {
        if (!luaIsTable(tableName)) {
            return 0.0;
        }

        if (!luaIsNil(variableName)) {
            return 0.0;
        }

        double result = lua_tonumber(L, -1);

        lua_pop(L, 2);

        return result;
    }

    std::string toString(const std::string &tableName, const std::string &variableName) {
        if (!luaIsTable(tableName)) {
            return " ";
        }

        if (!luaIsNil(variableName)) {
            return " ";
        }

        std::string result = lua_tostring(L, -1);

        lua_pop(L, 2);

        return result;
    }
protected:
    lua_State *L;

    bool luaIsTable(const std::string &tableName) {
        lua_getglobal(L, tableName.c_str());

        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);

            std::cerr << "Tabela lua não encontra." << std::endl;

            return false;
        }

        return true;
    }

    bool luaIsNil(const std::string &variableName) {
        lua_getfield(L, -1, variableName.c_str());

        if (lua_isnil(L, -1)) {
            lua_pop(L, 2);

            std::cerr << "Variável lua não encontrada." << std::endl;

            return false;
        }

        return true;
    }
};

#endif