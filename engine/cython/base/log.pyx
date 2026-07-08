from libcpp.string cimport string
from .log cimport Log as CLog

def info(msg: str):
    CLog.info(string(msg))

def warn(msg: str):
    CLog.warn(string(msg))

def error(msg: str):
    CLog.error(string(msg))
