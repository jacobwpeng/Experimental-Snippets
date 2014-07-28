'''
doc
'''

import os

class LuaCodeGenerator():
    '''
    generator that produces lua code in C
    '''

    def __init__(self, module_name = 'default'):
        self._module_name = module_name
        self._functions = []
        self._headers = []

    def add_functions(functions):
        if len(functions) > 0:
            assert isinstance(functions[0], CppFunction)
            self._functions += functions

    def add_headers(headers):
        if len(functions) > 0:
            assert isinstance(headers[0], CppHeader)
            self._headers += headers

    def do_code_generation():
        output = ''
        output += self._write_headers()

    def _write_headers():
        headers = []
        for header in self._headers:
            headers.append (str(header))

        return os.linesep.join (headers)
