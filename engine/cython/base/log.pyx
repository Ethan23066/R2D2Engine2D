from .log cimport Log as CLog

def info(msg: str):
    CLog.info(msg.encode('utf-8'))

def warn(msg: str):
    CLog.warn(msg.encode('utf-8'))

def error(msg: str):
    CLog.error(msg.encode('utf-8'))
