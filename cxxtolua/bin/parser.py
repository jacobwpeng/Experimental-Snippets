#!/usr/bin/env python

import clang.cindex

class SourcePosition:
    def __init__(self, filename, line, col):
        self.filename = filename
        self.line = line
        self.col = col

    def __str__(self):
        return '[%s %d:%d]' % (self.filename, self.line, self.col)

    def __hash__(self):
        return hash('%s:%d:%d' % (self.filename, self.line, self.col))

    def __eq__(self, other):
        return self.filename == other.filename and self.line == other.line and self.col == other.col

    def __ne__(self, other):
        return not self.__eq__(other)

class Type:

    def __init__(self):
        self.base_type = ''
        self.conversion_type = ''
        self.kind = ''
        #self.pointer_level = 0
        self.is_const = False
        self.is_ref = False

    def __str__(self):
        #tu = (self.real_type, self.conversion_type, self.kind, self.pointer_level, self.is_const)
        tu = (self.base_type, self.conversion_type, self.kind, self.is_const, self.is_ref)
        return '{}'.format(tu)

    @property
    def hashcode(self):
        return hash(self.conversion_type)


class ArgumentType:

    def __init__(self):
        self.type_ = Type()
        self.name = ''

    def __str__(self):
        return '%s, name: %s' % (self.type_, self.name)

class ResultType:

    def __init__(self):
        self.type_ = Type()

    def __str__(self):
        return str(self.type_)

class Function:

    def __init__(self):
        self._semantic_parents = []
        self._arguments = []
        self._result_type = ResultType()
        self._name = ''
        self._is_const = False
        self._is_class_member_method = False
        self._optional_arguments_count = 0

    def __str__(self):
        return 'name: %s' % self._name

    @property
    def arguments(self):
        return self._arguments

    def append_argument(self, arg):
        assert isinstance(arg, ArgumentType)
        self._arguments.append(arg)

    @property
    def semantic_parents(self):
        return self._semantic_parents

    def append_semantic_parent(self, parent):
        assert isinstance(parent, str)
        self._semantic_parents.append(parent)

    @property
    def result_type(self):
        return self._result_type

    def set_result_type(self, result_type):
        assert isinstance(result_type, ResultType)
        self._result_type = result_type

    @property
    def name(self):
        return self._name

    def set_name(self, name):
        assert isinstance(name, str)
        self._name = name

    @property
    def is_const(self):
        return self._is_const

    def set_const(self, flag):
        assert isinstance(flag, bool)
        self._is_const = flag

    @property
    def is_class_member_method(self):
        return self._is_class_member_method

    def set_class_member_method(self, flag):
        assert isinstance(flag, bool)
        self._is_class_member_method = flag

    @property
    def full_name(self):
        if not hasattr(self, '_full_name'):
            self._full_name = '%s::%s' % ('::'.join(self._semantic_parents), self._name)
        return self._full_name

    @property
    def fully_qualified_prefix(self):
        if not hasattr(self, '_fully_qualified_prefix'):
            self._fully_qualified_prefix  = '::'.join ([ prefix for prefix in self._semantic_parents ])
        return self._fully_qualified_prefix

    def set_optional_arguments_count(self, count):
        assert isinstance(count, int)
        self._optional_arguments_count = count

    @property
    def min_arguments_required(self):
        return len(self._arguments)  - self._optional_arguments_count

class FunctionParser:

    @staticmethod
    def parse_class_method(cursor):
        '''
        result: Function
        '''
        func = Function()
        func.set_name (cursor.spelling)
        (is_const, is_static) = FunctionParser._get_class_method_qualifier(cursor)
        func.set_const (is_const)
        func.set_class_member_method (not is_static)

        semantic_parents = FunctionParser._get_semantic_parents(cursor)
        for parent in semantic_parents:
            func.append_semantic_parent (parent)

        func.result_type = FunctionParser._get_result_type(cursor)

        args = FunctionParser._get_args_type(cursor)
        #prepend the class ptr as first argument
        args.insert (0, FunctionParser._get_class_ptr_type(cursor))

        for arg in args:
            func.append_argument (arg)

        optional_arguments_count = 0
        for c in cursor.get_children():
            if c.kind == clang.cindex.CursorKind.PARM_DECL and any( [token.spelling == '=' for token in c.get_tokens()] ):
                optional_arguments_count += 1
        func.set_optional_arguments_count(optional_arguments_count)

        return func

    @staticmethod
    def parse_free_function(cursor):
        '''
        result: function
        '''
        func = Function()
        func.set_name (cursor.spelling)

        semantic_parents = FunctionParser._get_semantic_parents(cursor)
        for parent in semantic_parents:
            func.append_semantic_parent (parent)

        func.result_type = FunctionParser._get_result_type(cursor)

        for arg in FunctionParser._get_args_type(cursor):
            func.append_argument (arg)

        return func

    @staticmethod
    def _get_class_method_qualifier(cursor):
        '''
        desc:   check whether a class method is const or static
        result: tuple(is_const, is_static)
        '''
        assert cursor.kind == clang.cindex.CursorKind.CXX_METHOD
        usr = cursor.get_usr()

        if usr.endswith('S'):
            return (False, True)
        elif usr[-1].isdigit():
            flag = int(usr[-1])
            is_const = False
            if flag & 0x1:
                is_const = True
            return (is_const, False)
        else:
            return (False, False)

    @staticmethod
    def _get_semantic_parents(cursor):
        '''
        result: [ semantic_parent ]
        '''
        parents = []
        sp = cursor.semantic_parent
        while not sp.kind.is_translation_unit():
            parents.append(sp.spelling)
            sp = sp.semantic_parent

        return [ parent for parent in reversed(parents) ]

    @staticmethod
    def _get_result_type(cursor):
        '''
        result: ResultType
        '''
        res = ResultType()
        canonical_type = cursor.result_type.get_canonical()

        res.type_ = FunctionParser._get_type(canonical_type)
        return res

    @staticmethod
    def _get_class_ptr_type(cursor):
        '''
        result: ArgumentType
        '''

        argument_type = ArgumentType()
        argument_type.name = '__class_ptr'
        canonical_type = cursor.semantic_parent.type.get_canonical()
        argument_type.type_ = FunctionParser._get_type(canonical_type)

        return argument_type

    @staticmethod
    def _get_args_type(cursor):
        '''
        result: [ ArgumentType ]
        '''

        args = []
        for idx, arg in enumerate(cursor.get_arguments()):
            argument_type = ArgumentType()
            argument_type.name = arg.spelling
            canonical_type = arg.type.get_canonical()
            argument_type.type_ = FunctionParser._get_type(canonical_type)

            args.append(argument_type)
        return args

    @staticmethod
    def _get_type(canonical_type):
        '''
        desc: get class Type from canonical_type
        '''
        assert isinstance(canonical_type, clang.cindex.Type)
        typename = canonical_type.kind.name

        type_ = Type()
        type_.kind = typename
        type_.is_ref = 'LVALUEREFERENCE' == typename

        (type_.base_type, type_.conversion_type, type_.is_const) = FunctionParser._get_conversion_type(canonical_type)
        return type_

    @staticmethod
    def _get_conversion_type(type_):
        '''
        desc: get conversion type of type_
              a conversion type is the representation of type_ in lua_State

        result: (base_type, conversion_type, is_const)
        '''
        assert isinstance(type_, clang.cindex.Type)

        level = 0
        obj = type_
        typename = obj.kind.name
        original_typename = typename
        while typename == 'POINTER' or typename == 'LVALUEREFERENCE':
            level += 1
            obj = obj.get_pointee()
            typename = obj.kind.name

        base_type = obj
        is_const = obj.is_const_qualified()

        if obj.kind.name == 'RECORD':
            #get base_type fully qualified name
            base_type_cursor  = base_type.get_declaration()
            #get base_type semantic_parents
            sp = base_type_cursor.semantic_parent
            semantic_parents = []
            while not sp.kind.is_translation_unit():
                semantic_parents.append(sp.spelling)
                sp = sp.semantic_parent

            semantic_parents.reverse()
            semantic_parents.append('%s %s' % (base_type_cursor.displayname, '*' * level))

            conversion_type = '::'.join (semantic_parents)
            base_type_name = conversion_type
            if original_typename == 'RECORD': conversion_type += '*'
        else:
            base_type_name = obj.kind.name.lower()
            conversion_type = '%s %s' % (base_type_name, '*' * level)

        if is_const:
            conversion_type = 'const %s' % conversion_type
        return base_type_name, conversion_type, is_const

    @staticmethod
    def _get_pointer_type_level(type_):
        assert isinstance(type_, clang.cindex.Type)
        typename = type_.kind.name

        if typename != 'POINTER' and typename != 'LVALUEREFERENCE':
            return 0
        return 1 + FunctionParser._get_pointer_type_level(type_.get_pointee())

if __name__ == '__main__':
    pass
