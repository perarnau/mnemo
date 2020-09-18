import ctypes as ct
from . import libmnemo

# Base types
mn_result = ct.c_int
mn_size = ct.c_size_t
mn_handle = ct.c_void_p
mn_key = ct.c_ulonglong

mn_reusedm = mn_handle

def _mn_get_function(method, argtypes=[], restype=mn_result):
    res = getattr(libmnemo, method)
    res.restype = restype
    res.argtypes = argtypes
    return res

libmn_reusedm_init = _mn_get_function("mnemo_reusedm_init", [mn_size], mn_reusedm)
libmn_reusedm_add = _mn_get_function("mnemo_reusedm_add", [mn_reusedm, mn_key])
libmn_reusedm_reset = _mn_get_function("mnemo_reusedm_reset", [mn_reusedm], None)
libmn_reusedm_fini = _mn_get_function("mnemo_reusedm_fini", [mn_reusedm], None)

class ReuseDM():

    def __init__(self, maxsize=0):
        self.handle = libmn_reusedm_init(maxsize)

    def add(self, key):
        return libmn_reusedm_add(self.handle, key)

    def reset(self):
        libmn_reusedm_reset(self.handle)

    def __del__(self):
        libmn_reusedm_fini(self.handle)

