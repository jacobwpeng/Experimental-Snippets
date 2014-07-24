int luaopen_array(lua_State * L)
{
    ${libfuncs}

    luaL_newlib(L, libfuncs);

    return 1;
}
