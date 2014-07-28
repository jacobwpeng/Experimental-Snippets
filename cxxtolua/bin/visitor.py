'''
doc
'''

from clang.cindex import *
from parser import SourcePosition, FunctionParser

class FunctionVisitor:
    '''
    visitor of translation unit
    '''

    def __init__(self):
        self._export_functions = {} # pos -> Function dict
        self._current_tu = None
        pass

    @property
    def export_functions(self):
        return self._export_functions.itervalues()

    def visit(self, tu, node):
        assert isinstance(tu, TranslationUnit)
        assert isinstance(node, Cursor)

        self._current_tu = tu

        if node.kind.is_preprocessing():
            self._visit_preprocessing(node)
        elif node.kind == CursorKind.CXX_METHOD:
            self._visit_cxx_method(node)
        elif node.kind == CursorKind.FUNCTION_DECL:
            self._visit_function_decl(node)
        elif node.kind == CursorKind.CONSTRUCTOR:
            self._visit_constructor(node)
        else:
            pass

    def _visit_preprocessing(self, node):
        if node.kind == CursorKind.MACRO_INSTANTIATION and node.displayname == 'LUA_EXPORT':
            pos = SourcePosition(self._current_tu.spelling, node.location.line, node.location.column)
            self._export_functions[pos] = None # placeholder

    def _visit_cxx_method(self, node):
        pos = self._get_next_position(node)
        if pos:
            self._export_functions[pos] = FunctionParser.parse_class_method(node)

    def _visit_function_decl(self, node):
        pos = self._get_next_position(node)
        if pos:
            self._export_functions[pos] = FunctionParser.parse_free_function(node)

    def _visit_constructor(self, node):
        pos = self._get_next_position(node)
        if pos:
            self._export_functions[pos] = FunctionParser.parse_constructor(node)

    def _get_next_position(self, node):
        here = SourcePosition(self._current_tu.spelling, node.location.line, node.location.column)
        for pos, function in self._export_functions.iteritems():
            if function is None and pos < here:
                return pos
        return None

if __name__ == '__main__':
    pass
