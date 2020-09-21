import ctypes as ct
from ctypes.util import find_library
import os

mnemopath = os.environ.get("LIBMNEMO_SO", find_library("mnemo"))
assert mnemopath is not None
libmnemo = ct.cdll.LoadLibrary(mnemopath)

from .reuse import *
