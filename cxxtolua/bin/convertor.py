#!/usr/bin/env python

import os
import parser
from mako.template import Template

template_path = '/home/work/repos/test/cxxtolua/template/'

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
        arg_name = get_arg_name(arg, idx)
        kind = arg.type_.kind
        if is_string_type(arg):
            pass
        elif kind == 'RECORD' or kind == 'LVALUEREFERENCE':
            arg_name = '*%s' % arg_name
        arg_names.append(arg_name)

    arg_name_str = ', '.join(arg_names)
    if func.is_class_member_method:
        template = Template( ''' __class_ptr->${func_name}(${args}) ''')
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
        elif arg.type_.kind == 'RECORD':
            return True
    else:
        pass

    return False

def is_primitive_type(arg):
    assert (isinstance(arg, parser.ArgumentType) or isinstance(arg, parser.ResultType))
    return arg.type_.kind in primitive_types

def get_primitive_type_convert_func(arg):
    assert is_primitive_type(arg)
    assert isinstance(arg, parser.ArgumentType)

    return convert_funcs[arg.type_.kind]

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
            , convert_func = convert_funcs[arg.type_.kind])

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
    if res.type_.kind == 'LVALUEREFERENCE':
        template = Template('res->val = (void*) &( ${func_call} );') 
        template_str = template_str % template.render(func_call = func_call)
    elif res.type_.kind == 'POINTER':
        template = Template('res->val = (void*) ( ${func_call} );')
        template_str = template_str % template.render(func_call = func_call)
    elif res.type_.kind == 'RECORD':
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
    return template.render( conversion_type = res.type_.conversion_type, func_call = func_call, push_func = push_funcs[res.type_.kind])


class FunctionConvertor:
    '''
    Convert class Function to code in cplusplus
    '''

    #@staticmethod
    #def write_function(func):
    #    assert isinstance(func, parser.Function)

    #    if func.is_class_member_method:
    #        return FunctionConvertor._write_class_method(func)
    #    else:
    #        return FunctionConvertor._write_common_method(func)

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
        class_ptr_type.type_.kind = 'RECORD'

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

    @staticmethod
    def write_function(func):
        assert isinstance(func, parser.Function)
        #assert func.is_class_member_method

        args_declaration = [] #list of string
        args_check = [] #also list of string
        args_count = len(func.arguments)
        userdata_arg_assignment_template = Template('${arg_name} = (${conversion_type})(__ud->val);')
        primitive_arg_assignment_tempalte = Template('${arg_name} = ${lua_tofunc}(L, ${idx});')

        check_ud_template = Template(filename = template_path + 'check_userdata_template.c')
        check_primitive_tempalte = Template(filename = template_path + 'check_primitive_type_template.c')
        check_string_template = Template(filename = template_path + 'check_string_type_template.c')

        args_declaration.append ('UserdataWrapper * __ud;')
        for idx, arg in enumerate(func.arguments):
            real_idx = idx + 1 #starts from 1
            arg_name = get_arg_name(arg, real_idx)
            if is_string_type(arg):
                args_declaration.append ('const char * %s;' % arg_name)
            else:
                args_declaration.append ('%s %s;' % (arg.type_.conversion_type, arg_name))

            failed = 'return luaL_error(L, "arg %%d type mismatch!", %d);' % real_idx
            if is_string_type(arg):
                check_c_string_succeed = Template('${arg_name} = luaL_checkstring(L, ${idx});').render (
                                    arg_name = arg_name, idx = real_idx)
                check_std_string_succeed = Template('${arg_name} = ((${conversion_type})(__ud->val))->c_str();').render (
                                    arg_name = arg_name, conversion_type = arg.type_.conversion_type)
                args_check.append (check_string_template.render (idx = real_idx, 
                                    std_string_hashcode = hash('std::basic_string<char> *'),
                                    check_c_string_succeed = check_c_string_succeed,
                                    check_std_string_succeed = check_std_string_succeed, 
                                    check_failed = failed) )
            elif is_userdata_type(arg):
                succeed = userdata_arg_assignment_template.render (conversion_type = arg.type_.conversion_type, arg_name = arg_name)
                check_const_expression = ''
                if real_idx == 1 and func.is_const == False: check_const_expression = '__ud->type_info.is_const == false'
                args_check.append (check_ud_template.render (idx = real_idx, type_hash = arg.type_.hashcode, 
                    check_succeed = succeed, check_failed = failed, const_check = check_const_expression))

            elif is_primitive_type(arg):
                expression = '${arg_name} = ${convert_func}(L, ${idx});'
                args_check.append (check_primitive_tempalte.render (
                    check_expression = Template(expression).render (arg_name = arg_name, convert_func = get_primitive_type_convert_func(arg), idx = real_idx))
                    )
            else:
                print func.name, idx, arg
                assert False

        args_declaration_part = os.linesep.join (args_declaration)
        args_check_part = os.linesep.join (args_check)

        func_call = get_func_call(func, func.arguments)

        type_kind = func.result_type.type_.kind
        push_result_expression = ''
        if func.result_type.type_.kind == 'VOID':
            #no return type
            func_call_and_assignment_expression = '%s;' % func_call
        elif is_string_type(func.result_type):
            if type_kind == 'RECORD' or type_kind == 'LVALUEREFERENCE':
                template = Template(filename = template_path + 'generate_string_result_from_std_string.c')
            else:
                template = Template(filename = template_path + 'generate_string_result_from_c_string.c')
            func_call_and_assignment_expression = template.render (func_call = func_call)
            push_result_expression = Template('${pushfunc}(L, __res);').render (pushfunc = 'lua_pushstring')
        elif is_userdata_type (func.result_type):
            needs_gc = False
            if type_kind == 'RECORD':
                needs_gc = True
                template = Template(filename = template_path + 'generate_userdata_result.c')
            elif type_kind == 'LVALUEREFERENCE':
                template = Template(filename = template_path + 'generate_ref_userdata_result.c')
            elif type_kind == 'POINTER':
                template = Template(filename = template_path + 'generate_ptr_userdata_result.c')
            else:
                assert False
            func_call_and_assignment_expression = template.render (is_const = func.result_type.type_.is_const and 'true' or 'false'
                    , needs_gc = needs_gc and 'true' or 'false'
                    , hashcode = func.result_type.type_.hashcode, base_type = func.result_type.type_.base_type, func_call = func_call
                    , set_metatable_expression = '')
        elif is_primitive_type(func.result_type):
            template = Template(filename = template_path + 'generate_primitive_result.c')
            func_call_and_assignment_expression = template.render (conversion_type = func.result_type.type_.conversion_type, func_call = func_call)
            #only primitive type needs push result
            push_result_expression = Template('${pushfunc}(L, __res);').render (pushfunc = push_funcs[func.result_type.type_.kind])
        else:
            print func.name, func.result_type
            assert False

        result_count = func.result_type.type_.kind == 'VOID' and 0 or 1

        #print args_declaration_part
        #print args_check_part
        #print func_call_and_assignment_expression
        #print push_result_expression
        #print result_count

        function_body = Template (filename = template_path + 'function_body.c')
        print function_body.render (args_declarations = args_declaration_part, args_check = args_check_part
                , function_call_and_result_assignment = func_call_and_assignment_expression
                , push_result = push_result_expression
                , result_count = result_count)

        print '*' * 80
init()
