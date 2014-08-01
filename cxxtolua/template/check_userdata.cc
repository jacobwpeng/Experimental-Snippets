if (!__check_failed 
        && lua_type(L, ${narg}) == LUA_TUSERDATA 
        && (__ud = (UserdataWrapper*)lua_touserdata(L, ${narg})) 
        && __ud->type_info.hash == ${hashcode}
        ${check_const})
{
    ${arg_name} = (${cpp_type})(__ud->val);
}
else
{
    __check_failed = true;
}

