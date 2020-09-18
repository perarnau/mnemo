import ctypes as ct
import os

if os.environ.get("LIBMNEMO_SO"):
    libmnemo = ct.cdll.LoadLibrary(os.environ.get("LIBMNEMO_SO"))
else:
    libmnemo = ct.cdll.LoadLibrary("libreuse.so.0.0")

from .reuse import *
