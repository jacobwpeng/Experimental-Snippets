if (!__check_failed)
{
    int __isnum;
    ${arg_name} = lua_tonumberx(L, ${narg}, &__isnum);
    if (!__isnum) __check_failed = true;
}

