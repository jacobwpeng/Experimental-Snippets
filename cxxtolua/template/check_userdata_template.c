if (lua_type(L) == LUA_TUSERDATA && (__ud = (UserdataWrapper*)luatouserdata(L, ${idx}) && __ud->type_info.hash == ${type_hash} ${const_check})
{
    ${check_succeed}
}
else
{
    ${check_failed}
}
