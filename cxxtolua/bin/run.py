#!/usr/bin/env python

import sys
import clang.cindex
from clang.cindex import CursorKind
import parser
import convertor
from parser import SourcePosition
from mako.template import Template

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

def ProcessLuaExport(tu, node):
    #this translation unit only
    if node.location.file and str(node.location.file) != tu.spelling:
        return

    #LUA_EXPORT only
    if node.kind.is_preprocessing() and node.kind == clang.cindex.CursorKind.MACRO_INSTANTIATION and node.displayname == 'LUA_EXPORT':
        pos = SourcePosition(tu.spelling, node.location.line, node.location.column)
        exports[pos] = None

    #unresolved export
    if any(exports[key] == None for key in exports):
        for pos, v in exports.items():
            if v is None and (node.location.line > pos.line or node.location.line == pos.line and node.location.column > pos.col):
                func = None
                if node.kind == clang.cindex.CursorKind.CXX_METHOD:
                    func = parser.FunctionParser.parse_class_method(node)
                elif node.kind == clang.cindex.CursorKind.FUNCTION_DECL:
                    func = parser.FunctionParser.parse_free_function(node)
                elif node.kind == clang.cindex.CursorKind.CONSTRUCTOR:
                    func = parser.FunctionParser.parse_constructor(node)
                    pass
                else:
                    pass
                if not func is None: 
                    exports[pos] = func
                break

    for c in node.get_children():
        ProcessLuaExport(tu, c)

def main():
    import os
    filename = sys.argv[1]
    index = clang.cindex.Index.create()
    parse_options = clang.cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD                                                                        
    tu = index.parse(filename, ['-x', 'c++', '-std=c++11', '-D__CODE_GENERATOR__'], options=parse_options)

    ProcessLuaExport(tu, tu.cursor)
    funcs = {}
    for pos, func in exports.items():
        if func: 
            import copy
            names = copy.deepcopy(func.semantic_parents)
            names.append(func.name)
            key = '_'.join(names)
            if key in funcs:
                funcs[key].append (func)
            else:
                funcs[key] = [func]

    libfuncs = {}
    instfuncs = {}
    for key, overload_funcs in funcs.iteritems():
        convertor.FunctionConvertor.write_function(overload_funcs)
        func = overload_funcs[0]
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

if __name__ == '__main__':
    main()
