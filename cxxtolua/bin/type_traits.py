'''
map types between lua and cplusplus
'''

import cpp_types

def is_integer_type(type_):
    return type_.kind in ["BOOL", "CHAR_U", "UCHAR", "CHAR16", "CHAR32", "USHORT", "UINT", "ULONG", "ULONGLONG", "UINT128", 
            "CHAR_S", "SCHAR", "WCHAR", "SHORT", "INT", "LONG", "LONGLONG", "INT128"]

def is_real_number_type(type_):
    return type_.kind in ['FLOAT', 'DOUBLE', 'LONGDOUBLE']

def is_c_string_type(type_):
    return type_.cpp_type == 'const char *'

def is_std_string_type(type_):
    return type_.base_type == 'std::basic_string<char>' and (type_.is_class or (type_.is_ref and type_.is_const))

def is_string_type(type_):
    assert isinstance(type_, cpp_types.Type)
    return is_c_string_type(type_) or is_std_string_type(type_)

def is_signed_integer_type(type_):
    return type_.kind in ["CHAR16", "CHAR32", "CHAR_S", "SCHAR", "WCHAR", "SHORT", "INT", "LONG", "LONGLONG", "INT128"]

def is_unsigned_integer_type(type_):
    return type_.kind in ['CHAR_U', 'UCHAR', 'USHORT', 'UINT', 'ULONG', 'ULONGLONG', 'UINT128']

def is_userdata_type(type_):
    return not is_integer_type(type_) and not is_real_number_type(type_) and not is_string_type(type_)
