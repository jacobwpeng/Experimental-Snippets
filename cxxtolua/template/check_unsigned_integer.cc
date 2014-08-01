if (!__check_failed)
{
    int __isnum;
    ${arg_name} = lua_tounsignedx(L, ${narg}, &__isnum);
    if (!__isnum) __check_failed = true;
}

