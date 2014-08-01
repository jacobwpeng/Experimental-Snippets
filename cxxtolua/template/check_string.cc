if (!__check_failed && lua_type(L, ${narg}) == LUA_TSTRING)
{
    ${arg_name} = luaL_checkstring(L, ${narg});
}
else if (!__check_failed 
        && lua_type(L, ${narg}) == LUA_TUSERDATA 
        && (__ud = (UserdataWrapper*)lua_touserdata(L, ${narg})) 
        && __ud->type_info.hash == ${std_string_hashcode})
{
    ${arg_name} = *((std::string *)(__ud->val));
}
else
{
    __check_failed = true;
}
