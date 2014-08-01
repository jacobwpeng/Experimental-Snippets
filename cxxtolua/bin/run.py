#!/usr/bin/env python

import sys
import clang.cindex
from visitor import FunctionVisitor
from generator import LuaCodeGenerator

exports = {}
objects = {}

def ParseExportFunction(visitor, tu, node):
    #this translation unit only
    if node.location.file and str(node.location.file) != tu.spelling:
        return

    visitor.visit(tu, node)

    for child in node.get_children():
        ParseExportFunction(visitor, tu, child)

def parse():
    argc = len(sys.argv)
    visitor = FunctionVisitor()
    for filename in [sys.argv[idx] for idx in xrange(1, argc)]:
        index = clang.cindex.Index.create()
        parse_options = clang.cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD                                                                        
        tu = index.parse(filename, ['-x', 'c++', '-std=c++11', '-D__CODE_GENERATOR__'], options = parse_options)
        ParseExportFunction(visitor, tu, tu.cursor)

    return visitor.scope

def visit_scope(visitor, scope):
    visitor.visit_scope(scope)
    for sub in scope.sub_scopes:
        visit_scope(visitor, sub)

def main():
    scope = parse()
    code_generator = LuaCodeGenerator()
    visit_scope(code_generator, scope)

if __name__ == '__main__':
    main()
