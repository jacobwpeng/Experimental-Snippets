'''
map types between lua and cplusplus
'''

import cpp_types

def is_integer_type(type_):
    return type_.kind in ["BOOL", "CHAR_U", "UCHAR", "CHAR16", "CHAR32", "USHORT", "UINT", "ULONG", "ULONGLONG", "UINT128", 
            "CHAR_S", "SCHAR", "WCHAR", "SHORT", "INT", "LONG", "LONGLONG", "INT128"]

def is_real_number_type(type_):
    return type_.kind in ['FLOAT', 'DOUBLE', 'LONGDOUBLE']


def is_string_type(type_):
    assert isinstance(type_, cpp_types.Type)
    if type_.base_type == 'std::basic_string<char>':
        if type_.is_class or (type_.is_ref and type_.is_const):
            #const string& and string
            return True
        else:
            return False
    elif type_.cpp_type == 'const char *':
        return True
    else:
        return False

def is_signed_integer_type(type_):
    return type_.kind in ["CHAR16", "CHAR32", "CHAR_S", "SCHAR", "WCHAR", "SHORT", "INT", "LONG", "LONGLONG", "INT128"]

def is_unsigned_integer_type(type_):
    return type_.kind in ['CHAR_U', 'UCHAR', 'USHORT', 'UINT', 'ULONG', 'ULONGLONG', 'UINT128']
