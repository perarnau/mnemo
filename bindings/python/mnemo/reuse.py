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

__libmn_reusedm_init = getfunction("mnemo_reusedm_init", [mn_size], mn_reusedm)
__libmn_reusedm_add = getfunction("mnemo_reusedm_add", [mn_reusedm, mn_key])
__libmn_reusedm_reset = getfunction("mnemo_reusedm_reset", [mn_reusedm], None)
__libmn_reusedm_fini = getfunction("mnemo_reusedm_fini", [mn_reusedm], None)

class ReuseDM():

    def __init__(self, maxsize=0):
        self.handle = __libmn_reusedm_init(maxsize)

    def add(self, key):
        return __libmn_reusedm_add(self.handle, key)

    def reset(self):
        __libmn_reusedm_reset(self.handle)

    def __del__(self):
        __libmn_reusedm_fini(self.handle)

