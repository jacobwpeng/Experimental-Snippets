'''
lua code generator
'''

import os
import cpp_types
import type_traits

from mako.template import Template

template_path = '/home/work/repos/test/cxxtolua/template/'

def to_cpp_bool(b):
    assert isinstance(b, bool)
    return b and 'true' or 'false'

def sort_arg(one, other):
    assert isinstance(one, cpp_types.FunctionArgument)
    assert isinstance(other, cpp_types.FunctionArgument)

    if type_traits.is_integer_type(one.type_):
        return -1
    if type_traits.is_integer_type(other.type_):
        return 1

    if type_traits.is_real_number_type(one.type_):
        return -1
    if type_traits.is_real_number_type(other.type_):
        return 1

    if type_traits.is_string_type(one.type_):
        return -1
    if type_traits.is_string_type(other.type_):
        return 1

    return 0

def sort_simple_functions(one, other):
    assert isinstance(one, cpp_types.SimpleFunction)
    assert isinstance(other, cpp_types.SimpleFunction)

    if one.argc == other.argc:
        res = 0
        for idx in xrange(0, one.argc):
            res = sort_arg(one.parent.arguments[idx], other.parent.arguments[idx])
            if res != 0:   break 
        return res

    return one.argc < other.argc and -1 or 1

def get_argument_name(arg, idx):
    arg_name = arg.name and arg.name or ('arg%d' % idx)
    return arg_name

def get_arguments_declaration(function):
    assert isinstance(function, cpp_types.SimpleFunction)

    fmt = '%s %s;'
    declarations = []
    for idx, arg in enumerate([ function.parent.arguments[idx] for idx in xrange(0, function.argc) ]):
        arg_name = get_argument_name(arg, idx)
        if type_traits.is_string_type(arg.type_):
            cpp_type = 'std::string'
        else:
            cpp_type = arg.type_.cpp_type
        declarations.append (fmt % (cpp_type, arg_name))

    if function.parent.proto.kind == cpp_types.FunctionKind.ClassMemberMethod:
        if function.parent.is_const:
            class_ptr_decl = 'const %s * __class_ptr;' % function.parent.proto.fully_qualified_valid_variable_prefix
        else:
            class_ptr_decl = '%s * __class_ptr;' % function.parent.proto.fully_qualified_valid_variable_prefix
        declarations.append (class_ptr_decl)

    return os.linesep.join(declarations)

def check_arguments(function):
    assert isinstance(function, cpp_types.SimpleFunction)
    stmts = []
    arg_offset = 1

    if function.parent.proto.kind == cpp_types.FunctionKind.ClassMemberMethod:
        template = Template(filename = template_path + 'check_userdata_argument.cc')
        raw_cpp_type = '%s *' % function.parent.proto.fully_qualified_prefix
        stmt = template.render(arg = '__class_ptr', idx = arg_offset, hashcode = hash(raw_cpp_type))
        stmts.append(stmt)
        arg_offset += 1

    for idx, arg in enumerate([ function.parent.arguments[idx] for idx in xrange(0, function.argc) ]):
        arg_name = get_argument_name(arg, idx)
        narg = idx + arg_offset

        if type_traits.is_userdata_type(arg.type_):
            template = Template(filename = template_path + 'check_userdata_argument.cc')
            stat = template.render(arg = arg_name, idx = narg, hashcode = arg.type_.hashcode)
            stmts.append (stat)
        else:
            template = Template(filename = template_path + 'check_scalar_argument.cc')
            stat = template.render(arg = arg_name, idx = narg)
            stmts.append (stat)
    return stmts
    #return os.linesep.join(stmts)

def get_arguments_in_function_call(function):
    assert isinstance(function, cpp_types.SimpleFunction)
    args = []
    for idx, arg in enumerate([function.parent.arguments[idx] for idx in xrange(0, function.argc)]):
        assert isinstance(arg, cpp_types.FunctionArgument)
        argname = get_argument_name(arg, idx)
        if type_traits.is_c_string_type(arg.type_):
            args.append ('%s.c_str()' % argname)
        elif type_traits.is_std_string_type(arg.type_):
            args.append('%s' % argname)
        elif type_traits.is_userdata_type(arg.type_) and (arg.type_.is_ref or arg.type_.is_class):
            args.append('*%s' % argname)
        else:
            args.append('%s' % argname)
    return ', '.join(args)

def get_function_call(function):
    assert isinstance(function, cpp_types.SimpleFunction)
    default_method_template = Template('${funcname}(${args})')
    class_method_template = Template('__class_ptr->${funcname}(${args})')

    args = get_arguments_in_function_call(function)

    if function.parent.proto.kind == cpp_types.FunctionKind.ClassMemberMethod:
        function_call = class_method_template.render (funcname = function.parent.proto.name, args = args)
    else:
        function_call = default_method_template.render (funcname = function.parent.proto.fully_qualified_name, args = args)

    return function_call

def get_push_result(function, function_call):
    assert isinstance(function, cpp_types.SimpleFunction)
    type_ = function.parent.result.type_
    if type_.is_void:
        return '%s;%sreturn 0;' % (function_call, os.linesep)
    elif type_traits.is_userdata_type(type_):
        mt_name = type_.base_type_as_variable
        if type_.is_ptr:
            return Template('return PushResult(L, ${function_call}, ${is_const}, ${hashcode}, ${mt_name});').render(
                    function_call = function_call, is_const = to_cpp_bool(type_.is_const)
                    , hashcode = type_.hashcode, mt_name = mt_name)
        else:
            return Template('return PushResult(L, ${function_call}, ${needs_gc}, ${is_const}, ${hashcode}, ${mt_name});').render(
                    function_call = function_call, needs_gc = to_cpp_bool(type_.is_class), is_const = to_cpp_bool(type_.is_const)
                    , hashcode = type_.hashcode, mt_name = mt_name)
    else:
        return Template('return PushResult(L, (${function_call}));').render(function_call = function_call)

def write_functions(functions):
    assert isinstance(functions, list)
    assert len(functions) > 0
    function = functions[0]
    assert isinstance(function, cpp_types.SimpleFunction)

    sorted_functions = sorted(functions, sort_simple_functions)
    if function.parent.proto.kind == cpp_types.FunctionKind.FreeFunction:
        write_free_function(sorted_functions)

def write_single_simple_function(function):
    assert isinstance(function, cpp_types.SimpleFunction)
    declarations = get_arguments_declaration(function)

    check_stmts = check_arguments(function)
    function_call = get_function_call(function)
    push_result = get_push_result(function, function_call)
    print declarations
    if_stmt = Template('if (${cond}) { ${passed} }')
    print if_stmt.render(cond = (' && ').join(check_stmts), passed = push_result)
    print '*' * 80

def write_free_function(functions):
    for function in functions:
        write_single_simple_function(function)

def write_constructor(functions):
    pass

class LuaCodeGenerator:
    '''
    generator that produces lua code in Cplusplus
    '''

    def __init__(self, module_name = 'default'):
        self._module_name = module_name
        self._init_code = []
        self._function_code = []

    def visit_scope(self, scope):
        assert isinstance(scope, cpp_types.Scope)
        print '*' * 40 + scope.full_name + '*' * 40

        if isinstance(scope, cpp_types.NamespaceScope):
            self.visit_namespace_scope(scope)
        elif isinstance(scope, cpp_types.ClassScope):
            self.visit_class_scope(scope)
        else:
            assert False, scope.full_name

    def visit_namespace_scope(self, scope):
        namespace_init_code_template = Template(filename = template_path + 'scope_init_code.cc')
        if scope.name != '__global':
            init_code = namespace_init_code_template.render(scope_name = scope.full_name)
            self._init_code.append (init_code)
            for simple_functions in scope.free_functions.itervalues():
                self._function_code.append (write_functions(simple_functions))

    def visit_class_scope(self, scope):
        class_init_code_template = Template(filename = template_path + 'class_init_code.cc')
        init_code = class_init_code_template.render(scope_name = scope.full_name)
        #print init_code
        self._init_code.append (init_code)

    def do_code_generation():
        output = ''
        output += self._write_headers()

__all__ = ['LuaCodeGenerator']
