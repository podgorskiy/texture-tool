# import sys
# sys.path.insert(0, "cmake-build-debug/")
# sys.path.insert(0, "cmake-build-release/")


def _load_libpvr():
    import os
    import platform
    import ctypes
    lib_name = 'libPVRTexLib' + ('.dylib' if platform.system() == 'Darwin' else '.so')
    here = os.path.abspath(__file__)
    lib_path = os.path.join(os.path.dirname(here), '../', lib_name)
    ctypes.CDLL(lib_path, mode=ctypes.RTLD_GLOBAL)


_load_libpvr()


from _pypvrtex import *

