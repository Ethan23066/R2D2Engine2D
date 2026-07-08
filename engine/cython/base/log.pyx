from libcpp.string cimport string
from .log cimport Log as CLog

def info(msg: str):
    cdef string cpp_msg = string(msg.encode('utf-8'))
    CLog.info(cpp_msg)

def warn(msg: str):
    cdef string cpp_msg = string(msg.encode('utf-8'))
    CLog.warn(cpp_msg)

def error(msg: str):
    cdef string cpp_msg = string(msg.encode('utf-8'))
    CLog.error(cpp_msg)
