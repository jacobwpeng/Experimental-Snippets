'''
results of parsing cplusplus types
'''

from collections import defaultdict

class Type:
    def __init__(self):
        self.base_type = 'base typename'
        self.cpp_type = 'type of variable declaration in c++'
        self.kind = 'type kind from libclang'
        self.is_const = 'whether base_type is const'

    @property
    def hashcode(self):
        return hash(self.cpp_type)

    @property
    def is_ref(self):
        return self.kind == 'LVALUEREFERENCE'

    @property
    def is_ptr(self):
        return self.kind == 'POINTER'

    @property
    def is_class(self):
        return self.kind == 'RECORD'

class FunctionResult:
    def __init__(self):
        self.type_ = Type()

class FunctionArgument:
    def __init__(self, name = None):
        self.type_ = Type()
        self.name = name

class FunctionKind:
    '''
    for now only support 4 different kinds
    1.free function
    2.class static method
    3.class member method
    4.constructor
    '''

    _name_map = None
    def __init__(self, value):
        self.kind = value

    @property
    def name(self):
        if self._name_map is None:
            self._name_map = {}
            for k, v in FunctionKind.__dict__.iteritems():
                if isinstance(v, FunctionKind):
                    self._name_map[v] = k
        return self._name_map[self]

    def __str__(self):
        return 'FunctionKind.%s' % self.name

    def __eq__(self, other):
        return self.kind == other.kind

    def __hash__(self):
        return self.kind

FunctionKind.FreeFunction = FunctionKind(1)
FunctionKind.ClassStaticMethod = FunctionKind(2)
FunctionKind.ClassMemberMethod = FunctionKind(3)
FunctionKind.Constructor = FunctionKind(4)

class FunctionProto:
    '''
    eg: int SayHello(int argc, double d = 1.0);
        void SayHello(const std::string& msg);

        we transform them into 3 simple functions

        int SayHello(int argc);
        int SayHello(int argc, double d);
        void SayHello(const std::string& msg);

        This three functions can be considered of the same function prototype
        since both of them have just the same semantic_parents and name

    a function proto is the prototype function of simple functions
    '''
    def __init__(self, name, kind):
        assert isinstance(kind, FunctionKind)
        self.name = name
        self.kind = kind
        self.semantic_parents = []

    @property
    def fully_qualified_prefix(self):
        return '::'.join (self.semantic_parents)

    @property
    def fully_qualified_name(self):
        return '::'.join ([p.name for p in self.semantic_parents] + [self.name])

    @property
    def fully_qualified_valid_variable_prefix(self):
        return '_'.join (self.semantic_parents)

class Function:
    '''
    a function is one of the unique forms of cplusplus functions/methods
    '''
    def __init__(self, proto):
        assert isinstance(proto, FunctionProto)
        self.proto = proto
        self.result = FunctionResult()
        self.arguments = []
        self.is_const = False

class SimpleFunction:
    '''
    a simple function is a cplusplus function that only accept concrete arguments type/count combination
    '''

    def __init__(self, parent, argc):
        self.parent = parent
        self.argc = argc

class SemanticParent:

    def __init__(self, kind, name):
        self.kind = kind
        self.name = name

class Scope:
    '''
    Semantic Scope in Cplusplus
    '''

    def __init__(self, prefix, name):
        self.prefix = prefix
        self.name = name
        self._sub_scopes = []

    @property
    def sub_scopes(self):
        return self._sub_scopes

    @property
    def full_name(self):
        if self.prefix:
            return '%s_%s' % (self.prefix, self.name)
        else:
            return self.name


    def get_scope(self, name):
        return next ((scope for scope in self._sub_scopes if scope.name == name), None)

class ClassScope(Scope):
    '''
    represents a class scope
    '''

    def __init__(self, prefix, name):
        Scope.__init__(self, prefix, name)
        self.constructors = []
        self.member_methods = []
        self.static_methods = []

    @property
    def sub_scopes(self):
        return self._sub_scopes

    def add_functions(self, functions):
        assert isinstance(functions, list)
        assert len(functions) > 0

        kind = functions[0].parent.proto.kind
        if kind == FunctionKind.Constructor:
            self.constructors.append (functions)
        elif kind == FunctionKind.ClassStaticMethod:
            self.static_methods.append (functions)
        elif kind == FunctionKind.ClassMemberMethod:
            self.member_methods.append (functions)
        else:
            assert False, kind

    def add_sub_class_scope(self, scope):
        assert isinstance(scope, ClassScope)
        self._sub_scopes.append (scope)

class NamespaceScope(Scope):
    '''
    represents a namespace scope
    '''

    def __init__(self, prefix, name):
        Scope.__init__(self, prefix, name)
        self._free_functions = defaultdict(list)
        self.sub_namespace_scopes = []

    def add_functions(self, functions):
        assert isinstance(functions, list)
        assert len(functions) > 0
        proto = functions[0].parent.proto
        assert functions[0].parent.proto.kind == FunctionKind.FreeFunction

        self._free_functions[proto.name] += functions

    @property
    def free_functions(self):
        return self._free_functions

    def add_sub_class_scope(self, scope):
        assert isinstance(scope, ClassScope)
        self._sub_scopes.append (scope)

    def add_sub_namespace_scope(self, scope):
        assert isinstance(scope, NamespaceScope)
        self._sub_scopes.append (scope)
