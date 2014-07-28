#!/usr/bin/env python

import os
import sys
import clang.cindex
import convertor
from clang.cindex import CursorKind
from mako.template import Template
from collections import defaultdict
from visitor import FunctionVisitor

exports = {}
objects = {}

def ProcessExportObject(tu, node):
    #this translation unit only
    if node.location.file and str(node.location.file) != tu.spelling:
        return

    if node.kind.is_preprocessing() and node.kind == clang.cindex.CursorKind.MACRO_INSTANTIATION and node.displayname == 'LUA_EXPORT_STACK_OBJECT':
        pos = SourcePosition(tu.spelling, node.location.line, node.location.column)
        objects[pos] = None

    decl_types = [CursorKind.STRUCT_DECL, CursorKind.CLASS_DECL]
    if node.kind in decl_types and any(class_ is None for class_ in objects.itervalues()):
        for pos, v in objects.items():
            if v is None and (node.location.line > pos.line or node.location.line == pos.line and node.location.column > pos.col):
                names = [node.displayname]
                sp = node.semantic_parent
                while not sp.kind.is_translation_unit():
                    names.append(sp.spelling)
                    sp = sp.semantic_parent

                names.reverse()
                objects[pos] = '::'.join(names)
                break

    for c in node.get_children():
        ProcessExportObject(tu, c)

def ProcessLuaExport(visitor, tu, node):
    #this translation unit only
    if node.location.file and str(node.location.file) != tu.spelling:
        return

    visitor.visit(tu, node)

    for c in node.get_children():
        ProcessLuaExport(visitor, tu, c)

def parse():
    filename = sys.argv[1]
    index = clang.cindex.Index.create()
    parse_options = clang.cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD                                                                        
    tu = index.parse(filename, ['-x', 'c++', '-std=c++11', '-D__CODE_GENERATOR__'], options = parse_options)

    visitor = FunctionVisitor()
    ProcessLuaExport(visitor, tu, tu.cursor)

    return visitor.export_functions

def code_generation(export_functions):
    functions_with_same_name = defaultdict(list)
    for func in export_functions:
        if func: 
            names = [p for p in func.semantic_parents] + [func.name]
            key = '_'.join(names)
            functions_with_same_name[key].append(func)

    #print any ([funcs[0].is_heap_constructor for funcs in functions_with_same_name.itervalues()])

    libfuncs = {}
    instfuncs = {}
    for key, functions in functions_with_same_name.iteritems():
        convertor.FunctionConvertor.write_function(functions)
        func = functions[0]
        if func.is_class_member_method:
            instfuncs[func.name] = func.fully_qualified_prefix.replace('::', '_') + '_' + func.name
        elif len(func.fully_qualified_prefix) != 0:
            libfuncs[func.name] = func.fully_qualified_prefix.replace('::', '_') + '_' + func.name
        else:
            libfuncs[func.name] = func.name

    fmt = '''
    static const struct luaL_Reg %s[] = {
        %s
        {NULL, NULL}
    };
    '''

    export_funcs = []
    for short_name, name in libfuncs.iteritems():
        export_funcs.append ('{"%s", %s},' % (short_name, name))
    #print fmt % ('libfuncs', os.linesep.join (export_funcs))

    #export_funcs = []
    for short_name, name in instfuncs.iteritems():
        export_funcs.append ('{"%s", %s},' % (short_name, name))
    libfuncs_export = fmt % ('libfuncs', os.linesep.join (export_funcs))

    export_library = Template(filename = '/home/work/repos/test/cxxtolua/template/export_library.cc').render (libfuncs = libfuncs_export)

    print export_library

def main():
    functions = parse()
    code_generation(functions)

if __name__ == '__main__':
    main()
