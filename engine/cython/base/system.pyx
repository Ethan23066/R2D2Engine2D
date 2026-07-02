from .system cimport System as CSystem

def init():
    CSystem.init()

def shutdown():
    CSystem.shutdown()

def is_initialized():
    return CSystem.is_initialized()
