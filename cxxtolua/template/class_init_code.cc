lua_newtable(L);

luaL_newmetatable(L, "${scope_name}_metatable");
lua_pushliteral(L, "__index");
lua_pushcfunction(L, ${scope_name}__index);
lua_settable(L, -3);
lua_pushliteral(L, "__newindex");
lua_pushcfunction(L, ${scope_name}__newindex);
lua_settable(L, -3);
lua_setmetatable(L, -2);
lua_setglobal(L, "${scope_name}");

luaL_newmetatable(L, "${scope_name}_instance_metatable");
lua_pushliteral(L, "__index");
lua_pushcfunction(L, ${scope_name}_instance__index);
lua_settable(L, -3);
lua_pushliteral(L, "__newindex");
lua_pushcfunction(L, ${scope_name}_instance__newindex);
lua_settable(L, -3);
lua_pop(L, 1);
