import ctypes as ct
from ctypes.util import find_library
import os

mnemopath = os.environ.get("LIBMNENO_SO", find_library("mnemo"))
libmnemo = ct.cdll.LoadLibrary(mnemopath)

from .reuse import *
