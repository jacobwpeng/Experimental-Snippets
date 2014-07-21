UserdataWrapper * __res = (UserdataWrapper*) lua_newuserdata(L, sizeof(UserdataWrapper));
__res->type_info.is_const = ${is_const};
__res->type_info.needs_gc = ${needs_gc};
__res->type_info.hash = ${hashcode};
__res->val = (void*) (new ${base_type}( ${func_call} ));
${set_metatable_expression}
