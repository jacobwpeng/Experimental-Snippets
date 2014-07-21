#!/usr/bin/env python

import os
import parser
from mako.template import Template

def get_arg_name(arg, idx):
    assert isinstance(idx, int)
    assert isinstance(arg, parser.ArgumentType)
    if arg.name:
        return arg.name
    else:
        return 'arg%d' % idx

def get_func_call(func, args):
    assert isinstance(func, parser.Function)
    assert isinstance(args, list)
    if len(args) != 0: assert isinstance(args[0], parser.ArgumentType)

    arg_names = []
    for idx, arg in enumerate(args):
        if func.is_class_member_method:
            idx += 2
        else:
            idx += 1

        arg_name = get_arg_name(arg, idx)
        kind = arg.type_.type_kind
        if is_string_type(arg):
            pass
        elif kind == 'RECORD' or kind == 'LVALUEREFERENCE':
            arg_name = '*%s' % arg_name
        arg_names.append(arg_name)

    arg_name_str = ', '.join(arg_names)
    if func.is_class_member_method:
        template = Template( ''' class_ptr->${func_name}(${args}) ''')
        return template.render(func_name = func.name, args = arg_name_str)
    else:
        return '%s::%s(%s) ' % (func.fully_qualified_prefix, func.name, arg_name_str)

def init():
    global primitive_types, convert_funcs, push_funcs
    primitive_types = {}
    convert_funcs = {}
    push_funcs = {}

    to_boolean = [ 'BOOL' ]
    to_unsigned = [ 'CHAR_U', 'UCHAR', 'USHORT', 'UINT' ]
    to_integer = [ 'ULONG', 'CHAR_S', 'SCHAR', 'WCHAR', 'SHORT', 'INT', 'LONG' ]
    to_number = [ 'FLOAT', 'DOUBLE' ]

    others = ['VOID']

    for type_ in to_boolean:
        primitive_types[type_] = True
        convert_funcs[type_] = 'lua_toboolean'
        push_funcs[type_] = 'lua_pushboolean'

    for type_ in to_unsigned:
        primitive_types[type_] = True
        convert_funcs[type_] = 'luaL_checkunsigned'
        push_funcs[type_] = 'lua_pushunsigned'

    for type_ in to_integer:
        primitive_types[type_] = True
        convert_funcs[type_] = 'luaL_checkinteger'
        push_funcs[type_] = 'lua_pushinteger'

    for type_ in to_number:
        primitive_types[type_] = True
        convert_funcs[type_] = 'luaL_checknumber'
        push_funcs[type_] = 'lua_pushnumber'

    for type_ in others:
        primitive_types[type_] = True


def is_userdata_type(arg):
    assert (isinstance(arg, parser.ArgumentType) or isinstance(arg, parser.ResultType))
    return arg.type_.conversion_type.endswith('*')

def is_string_type(arg):
    assert (isinstance(arg, parser.ArgumentType) or isinstance(arg, parser.ResultType))

    '''
    only 'cosnt string&, string, const char*'
    '''

    if arg.type_.conversion_type == 'const char_s *' and arg.type_.is_const:
        return True
    elif arg.type_.conversion_type == 'std::basic_string<char> *':
        if arg.type_.is_const and arg.type_.is_ref:
            return True
        elif arg.type_.type_kind == 'RECORD':
            return True
    else:
        pass

    return False

def is_primitive_type(arg):
    assert (isinstance(arg, parser.ArgumentType) or isinstance(arg, parser.ResultType))
    return arg.type_.type_kind in primitive_types

def get_primitive_type_convert_func(arg):
    assert is_primitive_type(arg)
    assert isinstance(arg, parser.ArgumentType)

    return convert_funcs[arg.type_.type_kind]

def convert_arg(arg, idx):
    assert isinstance(idx, int)
    assert isinstance(arg, parser.ArgumentType)
    if is_string_type(arg):
        return convert_arg_to_string_type(arg, idx)
    elif is_userdata_type(arg):
        return convert_arg_to_userdatatype(arg, idx)
    elif is_primitive_type(arg):
        return convert_arg_to_primitive_type(arg, idx)
    else:
        print arg, idx
        assert False

def convert_arg_to_string_type(arg, idx):
    assert isinstance(idx, int)
    assert isinstance(arg, parser.ArgumentType)
    assert is_string_type(arg)

    template_str = '''
    const char * ${arg_name} = NULL;
    if (lua_type(L, ${idx}) == LUA_TSTRING)
    {
        ${arg_name} = luaL_checkstring(L, ${idx});
    }
    else if (lua_type(L, ${idx}) == LUA_TUSERDATA)
    {
        ud = (${wrapper_type}*)(lua_touserdata(L, ${idx}));
        if (ud->type_info.hash != ${type_hash}) { ${arg_error} }

        ${arg_name} = (const char *) ((std::basic_string<char> *)ud->val)->c_str();
    }
    else
    {
        ${arg_error}
    }
    '''
    arg_error = 'return luaL_error(L, "arg %%d type mismatch", %d);' % idx

    template = Template(template_str)

    return template.render (arg_name = get_arg_name(arg, idx), idx = idx, wrapper_type = 'UserdataWrapper', type_hash = hash('std::basic_string<char> *')
            , arg_error = arg_error)

def convert_arg_to_userdatatype(arg, idx):
    assert isinstance(idx, int)
    assert isinstance(arg, parser.ArgumentType)
    assert is_userdata_type(arg)

    template_str = '''
    if (lua_type(L, ${idx}) != LUA_TUSERDATA) { ${arg_error} }
    else 
    {
        ud = (${wrapper_type}*)(lua_touserdata(L, 1));
        if (ud->type_info.hash != ${type_hash}${const_check_str}) { ${arg_error} }
    }
    ${conversion_type} ${arg_name} = (${conversion_type}) (ud->val);
    '''

    if not arg.type_.is_const:
        const_check_str = ' or ud->type_info.is_const'
    else:
        const_check_str = ''

    arg_error = 'return luaL_error(L, "arg %%d type mismatch", %d);' % idx
    template = Template(template_str)

    return template.render(idx = idx, arg_error  = arg_error, wrapper_type = 'UserdataWrapper', type_hash = hash(arg.type_.conversion_type)
            , conversion_type = arg.type_.conversion_type, arg_name = get_arg_name(arg, idx), const_check_str = const_check_str)

def convert_arg_to_primitive_type(arg, idx):
    assert isinstance(idx, int)
    assert isinstance(arg, parser.ArgumentType)
    assert is_primitive_type(arg)

    template_str = '''
    ${conversion_type} ${arg_name} = ${convert_func}(L, ${idx});
    '''

    template = Template(template_str)
    return template.render(idx = idx, conversion_type = arg.type_.conversion_type, arg_name = get_arg_name(arg, idx)
            , convert_func = convert_funcs[arg.type_.type_kind])

def convert_result(res, func_call):
    assert isinstance(func_call, basestring)
    assert isinstance(res, parser.ResultType)

    if is_userdata_type(res):
        return convert_result_to_userdata(res, func_call)
    elif is_primitive_type(res):
        return convert_result_to_primitive_type(res, func_call)
    else:
        print res, func_call
        assert False

def convert_result_to_userdata(res, func_call):
    assert isinstance(func_call, basestring)
    assert isinstance(res, parser.ResultType)
    assert is_userdata_type(res)

    template_str = '''
    ${wrapper_type} * res = (${wrapper_type}*) lua_newuserdata(L, sizeof(${wrapper_type}));
    res->type_info.is_const = ${is_const};
    res->type_info.needs_gc = ${needs_gc};
    res->type_info.hash = ${type_hash};
    %s

    return 1;
    '''

    needs_gc = 'false'
    if res.type_.type_kind == 'LVALUEREFERENCE':
        template = Template('res->val = (void*) &( ${func_call} );') 
        template_str = template_str % template.render(func_call = func_call)
    elif res.type_.type_kind == 'POINTER':
        template = Template('res->val = (void*) ( ${func_call} );')
        template_str = template_str % template.render(func_call = func_call)
    elif res.type_.type_kind == 'RECORD':
        template = Template('res->val = (void*) (new ${base_type} ( ${func_call} ));')
        template_str = template_str % template.render(base_type = res.type_.base_type, func_call = func_call)
        needs_gc = 'true'
    else:
        assert false

    template = Template(template_str)
    return template.render(wrapper_type = 'UserdataWrapper', is_const = res.type_.is_const and 'true' or 'false', needs_gc = needs_gc
            , type_hash = hash(res.type_.conversion_type) )

def convert_result_to_primitive_type(res, func_call):
    assert isinstance(func_call, basestring)
    assert isinstance(res, parser.ResultType)
    assert is_primitive_type(res)

    if res.type_.base_type == 'void':
        return '''
        %s;
        return 0;
        ''' % func_call

    template_str = '''
    ${conversion_type} res = (${func_call});
    ${push_func}(L, res);

    return 1;
    '''

    template = Template(template_str)
    return template.render( conversion_type = res.type_.conversion_type, func_call = func_call, push_func = push_funcs[res.type_.type_kind])


class FunctionConvertor:
    '''
    Convert class Function to code in cplusplus
    '''

    @staticmethod
    def write_function(func):
        assert isinstance(func, parser.Function)

        if func.is_class_member_method:
            return FunctionConvertor._write_class_method(func)
        else:
            return FunctionConvertor._write_common_method(func)

    @staticmethod
    def _write_class_method(func):
        assert isinstance(func, parser.Function)
        assert func.is_class_member_method
        full_func_name = func.name
        if func.semantic_parents:
            full_func_name = '%s_%s' % (func.semantic_parents[-1], func.name)

        class_ptr_type = parser.ArgumentType()
        class_ptr_type.name = 'class_ptr'
        class_ptr_type.type_.base_type = func.fully_qualified_prefix
        class_ptr_type.type_.conversion_type = '%s *' % (class_ptr_type.type_.base_type)
        class_ptr_type.type_.is_const = func.is_const
        class_ptr_type.type_.is_ref = False
        class_ptr_type.type_.type_kind = 'RECORD'

        stat = convert_arg_to_userdatatype(class_ptr_type, 1)

        for idx, arg in enumerate(func.arguments):
            stat += convert_arg(arg, idx + 2)
            stat += os.linesep

        func_call = get_func_call(func, func.arguments)

        stat += convert_result(func.result_type, func_call)

        template = Template('''
int ${full_func_name}(lua_State* L)
{
    UserdataWrapper * ud = NULL;
    ${stat}
}
        ''')

        return template.render(full_func_name = full_func_name, stat = stat)

    @staticmethod
    def _write_common_method(func):
        assert isinstance(func, parser.Function)
        assert not func.is_class_member_method

        full_func_name = '_'.join(func.semantic_parents + [func.name])

        stat = ''
        for idx, arg in enumerate(func.arguments):
            stat += convert_arg(arg, idx + 1)
            stat += os.linesep

        func_call = get_func_call(func, func.arguments)

        stat += convert_result(func.result_type, func_call)

        template = Template('''
int ${full_func_name}(lua_State* L)
{
    UserdataWrapper * ud = NULL;
    ${stat}
}
        ''')

        return template.render(full_func_name = full_func_name, stat = stat)

init()
