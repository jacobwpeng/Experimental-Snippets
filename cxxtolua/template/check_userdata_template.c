if (lua_type(L, ${idx}) == LUA_TUSERDATA 
        && (__ud = (UserdataWrapper*)lua_touserdata(L, ${idx})) 
        && __ud->type_info.hash == ${type_hash}
        ${const_check})
{
    ${check_succeed}
}
else
{
    ${check_failed}
}
