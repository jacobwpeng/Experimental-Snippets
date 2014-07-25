int ${function_name_prefix}${function_name}(lua_State * L)
{
    const int __top = lua_gettop(L);
    UserdataWrapper * __ud;
    ${function_bodies}

    return luaL_error(L, "Cannot find proper function to call.");
}
