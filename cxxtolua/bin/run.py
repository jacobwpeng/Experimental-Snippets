#!/usr/bin/env python

import sys
import clang.cindex
from clang.cindex import CursorKind
import parser
import convertor
from parser import SourcePosition

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

    #ProcessExportObject(tu, tu.cursor)
    #for decl in objects.itervalues():
    #    print decl
    print 'Translation unit:', tu.spelling
    ProcessLuaExport(tu, tu.cursor)

    #print '#include "userdata_wrapper.h"'
    #print '#include "dynamic_array.h"'
    #print '#include "lua.hpp"'
    #print 'extern "C" {'
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
            #print convertor.FunctionConvertor.write_function(func)
    #print '}'

    for key, overload_funcs in funcs.iteritems():
        print '%s has %d overload functions' % (key, len(overload_funcs))
        for func in overload_funcs:
            print 'func has %d arguments, %d required' % (len(func.arguments), func.min_arguments_required)
            print os.linesep.join([str(arg) for arg in func.arguments])
            print '*' * 80

if __name__ == '__main__':
    main()
