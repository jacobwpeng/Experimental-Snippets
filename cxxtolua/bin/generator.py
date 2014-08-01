'''
lua code generator
'''

import os
import cpp_types
import type_traits

from mako.template import Template

template_path = '/home/work/repos/test/cxxtolua/template/'

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

    #common declarations
    declarations.append ('UserdataWrapper * __ud = NULL;')
    declarations.append ('bool __check_failed = false;')
    if function.parent.proto.kind == cpp_types.FunctionKind.Constructor:
        class_ptr_decl = '%s * __class_ptr;' % function.parent.proto.fully_qualified_valid_variable_prefix
        if function.parent.is_const:
            class_ptr_decl = 'const %s' % class_ptr_decl
        declarations.append (class_ptr_decl)

    return os.linesep.join(declarations)

def check_arguments(function):
    assert isinstance(function, cpp_types.SimpleFunction)
    check_stmts = []
    arg_offset = 1

    if function.parent.proto.kind == cpp_types.FunctionKind.ClassMemberMethod:
        template = Template(filename = template_path + 'check_userdata.cc')
        check_const = ''
        raw_cpp_type = '%s *' % function.parent.proto.fully_qualified_prefix
        if not function.parent.is_const:
            check_const = '&& !__ud->type_info.is_const'
            cpp_type = raw_cpp_type
        else:
            cpp_type = 'const %s' % raw_cpp_type
        check_stmts.append (template.render (arg_name = '__class_ptr', narg = 1, cpp_type = cpp_type, 
            hashcode = hash(raw_cpp_type), check_const = check_const))
        arg_offset += 1

    for idx, arg in enumerate([ function.parent.arguments[idx] for idx in xrange(0, function.argc) ]):
        arg_name = get_argument_name(arg, idx)
        narg = idx + arg_offset
        if type_traits.is_signed_integer_type(arg.type_):
            template = Template(filename = template_path + 'check_signed_integer.cc')
            check_stmts.append (template.render(arg_name = arg_name, narg = narg))
        elif type_traits.is_unsigned_integer_type(arg.type_):
            template = Template(filename = template_path + 'check_unsigned_integer.cc')
            check_stmts.append (template.render(arg_name = arg_name, narg = narg))
        elif type_traits.is_real_number_type(arg.type_):
            template = Template(filename = template_path + 'check_real.cc')
            check_stmts.append (template.render(arg_name = arg_name, narg = narg))
        elif type_traits.is_string_type(arg.type_):
            template = Template(filename = template_path + 'check_string.cc')
            check_stmts.append (template.render(arg_name = arg_name, narg = narg, std_string_hashcode = hash('std::basic_string<char> *')))
        else:
            template = Template(filename = template_path + 'check_userdata.cc')
            check_const = ''
            if not arg.type_.is_const:
                check_const = '&& !__ud->type_info.is_const'
            check_stmts.append (template.render (arg_name = arg_name, narg = narg, cpp_type = arg.type_.cpp_type, 
                hashcode = arg.type_.hashcode, check_const = check_const))

    return os.linesep.join (check_stmts)

def write_simple_functions(functions):
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
    print check_stmts
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
                self._function_code.append (write_simple_functions(simple_functions))

    def visit_class_scope(self, scope):
        class_init_code_template = Template(filename = template_path + 'class_init_code.cc')
        init_code = class_init_code_template.render(scope_name = scope.full_name)
        #print init_code
        self._init_code.append (init_code)

    def do_code_generation():
        output = ''
        output += self._write_headers()

__all__ = ['LuaCodeGenerator']
