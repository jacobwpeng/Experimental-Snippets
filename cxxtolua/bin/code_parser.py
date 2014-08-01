'''
Parser of Cplusplus Code
'''

import cpp_types
import clang.cindex

m = {}

def get_cpp_type_of_primitive_type(kind):
    from clang.cindex import TypeKind
    assert isinstance(kind, TypeKind), kind
    if not m:
        m[TypeKind.VOID] = 'void'
        m[TypeKind.BOOL] = 'bool'
        m[TypeKind.CHAR_U] = 'unsigned char'
        m[TypeKind.UCHAR] = 'unsigned char'
        m[TypeKind.USHORT] = 'unsigned short'
        m[TypeKind.UINT] = 'unsigned int'
        m[TypeKind.ULONG] = 'unsigned long'
        m[TypeKind.CHAR_S] = 'char'
        m[TypeKind.SHORT] = 'short'
        m[TypeKind.INT] = 'int'
        m[TypeKind.LONG] = 'long'
        m[TypeKind.FLOAT] = 'float'
        m[TypeKind.DOUBLE] = 'double'
    #TypeKind.ENUM = TypeKind(106)

    return m[kind]

def get_semantic_parents(cursor):
    parents = []
    sp = cursor.semantic_parent
    if sp is None:  return []

    while not sp.kind.is_translation_unit():
        parents.append (cpp_types.SemanticParent(sp.kind, sp.spelling))
        sp = sp.semantic_parent

    return [ parent for parent in reversed(parents) ]

def get_type(canonical_type):
    assert isinstance(canonical_type, clang.cindex.Type)
    type_ = cpp_types.Type()
    type_.kind = canonical_type.kind.name

    level = 0
    cur = canonical_type
    cur_kind = cur.kind.name
    while cur_kind == 'POINTER' or cur_kind == 'LVALUEREFERENCE':
        level += 1
        cur = cur.get_pointee()
        cur_kind = cur.kind.name

    #whether basetype is const
    type_.is_const = cur.is_const_qualified()

    if cur_kind == 'RECORD':
        cur_kind_cursor  = cur.get_declaration()
        semantic_parents = get_semantic_parents(cur_kind_cursor)

        cpp_type = '::'.join([p.name for p in semantic_parents] + [cur_kind_cursor.displayname])
        base_type = cpp_type
        #add pointer/ref
        if type_.kind == 'RECORD':
            #original type is RECORD, then treat this as level 1 pointer
            level += 1
        cpp_type += ' %s' % ('*' * level)
    else:
        #lookup type map from clang.kind.name to cplusplus types
        base_type = get_cpp_type_of_primitive_type(cur.kind)
        cpp_type = base_type
        if level != 0:
            cpp_type += ' %s' % ('*' * level)

    type_.base_type = base_type
    type_.cpp_type = cpp_type

    if type_.is_const and level != 0:
        type_.cpp_type = 'const %s' % type_.cpp_type

    return type_

def get_result(cursor):
    assert isinstance(cursor, clang.cindex.Cursor)
    function_result = cpp_types.FunctionResult()
    canonical_type = cursor.result_type.get_canonical()

    function_result.type_ = get_type(canonical_type)
    return function_result

def get_arguments(cursor):
    assert isinstance(cursor, clang.cindex.Cursor)
    function_arguments = []

    for arg in cursor.get_arguments():
        function_argument = cpp_types.FunctionArgument(arg.spelling)
        function_argument.type_ = get_type(arg.type.get_canonical())
        function_arguments.append (function_argument)

    return function_arguments

def get_optional_arguments_count(cursor):
    assert isinstance(cursor, clang.cindex.Cursor)
    optional_arguments_count = 0
    for c in cursor.get_children():
        if c.kind == clang.cindex.CursorKind.PARM_DECL and any( [token.spelling == '=' for token in c.get_tokens()] ):
            optional_arguments_count += 1

    return optional_arguments_count

def get_class_method_qualifier(cursor):
    assert cursor.kind == clang.cindex.CursorKind.CXX_METHOD
    usr = cursor.get_usr()

    if usr.endswith('S'):
        return (False, True) # static method cannot be const
    elif usr[-1].isdigit():
        flag = int(usr[-1])
        is_const = False
        if flag & 0x1:
            is_const = True
        return (is_const, False)
    else:
        return (False, False)

def parse_simple_functions(cursor, kind):
    assert isinstance(cursor, clang.cindex.Cursor)
    assert isinstance(kind, cpp_types.FunctionKind)

    #TODO: Use FunctionProtoManager to reuse function proto object
    function_proto = cpp_types.FunctionProto(cursor.spelling, kind)
    function_proto.semantic_parents = get_semantic_parents(cursor)

    function = cpp_types.Function(function_proto)
    function.result = get_result(cursor)
    function.arguments = get_arguments(cursor)

    optional_argc = get_optional_arguments_count(cursor)
    total_argc = len(function.arguments)

    simple_functions = []
    for argc in xrange(total_argc - optional_argc, total_argc + 1):
        simple_functions.append (cpp_types.SimpleFunction(function, argc))

    return simple_functions

def parse_free_function(cursor):
    assert isinstance(cursor, clang.cindex.Cursor)
    assert cursor.kind == clang.cindex.CursorKind.FUNCTION_DECL
    return parse_simple_functions(cursor, cpp_types.FunctionKind.FreeFunction)

def parse_constructor(cursor):
    assert isinstance(cursor, clang.cindex.Cursor)
    assert cursor.kind == clang.cindex.CursorKind.CONSTRUCTOR
    return parse_simple_functions(cursor, cpp_types.FunctionKind.Constructor)

def parse_class_method(cursor):
    assert isinstance(cursor, clang.cindex.Cursor)
    assert cursor.kind == clang.cindex.CursorKind.CXX_METHOD
    (is_const, is_static) = get_class_method_qualifier(cursor)
    if is_static:
        return parse_class_static_method(cursor)
    else:
        return parse_class_member_method(cursor, is_const)

def parse_class_static_method(cursor):
    return parse_simple_functions(cursor, cpp_types.FunctionKind.ClassStaticMethod)

def parse_class_member_method(cursor, is_const):
    methods = parse_simple_functions(cursor, cpp_types.FunctionKind.ClassMemberMethod)
    methods[0].parent.is_const = is_const
    return methods

__all__ = ['get_semantic_parents', 'parse_class_method', 'parse_constructor', 'parse_free_function']
