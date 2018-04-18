import os, ctypes

if not "libgdbruby" in locals():
    libgdbruby = ctypes.cdll.LoadLibrary(os.path.expanduser(os.path.dirname(__file__)) + "/libgdbruby/libgdbruby.so")
