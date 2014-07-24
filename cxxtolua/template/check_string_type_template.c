if (lua_type(L, ${idx}) == LUA_TSTRING)
{
    ${check_c_string_succeed}
}
else if (lua_type(L, ${idx}) == LUA_TUSERDATA && (__ud = (UserdataWrapper*)lua_touserdata(L, ${idx})) && __ud->type_info.hash == ${std_string_hashcode})
{
    ${check_std_string_succeed}
}
else
{
    ${check_failed}
}
