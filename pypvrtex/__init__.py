import sys
import os

_here = os.path.abspath(__file__)
_here = os.path.dirname(_here)


def _load_libpvr():
    import platform
    import ctypes
    lib_name = 'libPVRTexLib' + ('.dylib' if platform.system() == 'Darwin' else '.so')
    lib_path = os.path.join(_here, '../', lib_name)
    ctypes.CDLL(lib_path, mode=ctypes.RTLD_GLOBAL)


# if running debug session
if os.path.exists("cmake-build-debug/"):
    sys.path.insert(0, "cmake-build-debug/")
    # sys.path.insert(0, "cmake-build-release/")
else:
    _load_libpvr()

del os
del sys


from _pypvrtex import *

