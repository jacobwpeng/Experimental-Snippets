'''
doc
'''

import cpp_types
import code_parser
from clang.cindex import *
from parser import SourcePosition

class FunctionVisitor:
    '''
    visitor of translation unit
    '''

    def __init__(self):
        self._export_functions = {} # pos -> Function dict
        self._current_tu = None
        self._global_scope = cpp_types.NamespaceScope('', '__global')
        self._process_map = {
                CursorKind.CXX_METHOD: self._visit_cxx_method,
                CursorKind.FUNCTION_DECL: self._visit_free_function,
                CursorKind.CONSTRUCTOR: self._visit_constructor
                }

    @property
    def scope(self):
        return self._global_scope

    def get_scope(self, node):
        assert isinstance(node, Cursor)
        semantic_parents = code_parser.get_semantic_parents(node)
        s = self._global_scope
        prefixes = []
        for p in semantic_parents:
            prev = s
            s = s.get_scope(p.name)
            if s is None:
                prefix = '_'.join(prefixes)
                if p.kind == CursorKind.CLASS_DECL or p.kind == CursorKind.STRUCT_DECL:
                    sub = cpp_types.ClassScope(prefix, p.name)
                    prev.add_sub_class_scope(sub)
                elif p.kind == CursorKind.NAMESPACE:
                    sub = cpp_types.NamespaceScope(prefix, p.name)
                    prev.add_sub_namespace_scope(sub)
                else:
                    assert False, p.kind
                s = prev.get_scope(p.name)
                prev = s
            assert s
            prefixes.append (s.name)

        return s


    def visit(self, tu, node):
        assert isinstance(tu, TranslationUnit)
        assert isinstance(node, Cursor)

        self._current_tu = tu

        if node.kind.is_preprocessing():
            self._visit_preprocessing(node)

        functions = None

        if node.kind in self._process_map:
            scope = self.get_scope(node)
            functions = self._process_map[node.kind](node)

        if functions:
            scope.add_functions(functions)

    def _visit_preprocessing(self, node):
        if node.kind == CursorKind.MACRO_INSTANTIATION and node.displayname == 'LUA_EXPORT':
            pos = SourcePosition(self._current_tu.spelling, node.location.line, node.location.column)
            self._export_functions[pos] = None # placeholder

    def _visit_cxx_method(self, node):
        pos = self._get_next_position(node)
        if pos:
            self._export_functions[pos] = True
            return code_parser.parse_class_method(node)

    def _visit_free_function(self, node):
        pos = self._get_next_position(node)
        if pos:
            self._export_functions[pos] = True
            return code_parser.parse_free_function(node)

    def _visit_constructor(self, node):
        pos = self._get_next_position(node)
        if pos:
            self._export_functions[pos] = True
            return code_parser.parse_constructor(node)

    def _get_next_position(self, node):
        here = SourcePosition(self._current_tu.spelling, node.location.line, node.location.column)
        for pos, function in self._export_functions.iteritems():
            if function is None and pos < here:
                return pos
        return None

if __name__ == '__main__':
    pass
