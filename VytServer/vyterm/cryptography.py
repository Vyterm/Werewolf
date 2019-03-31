import hashlib


def md5(src: bytes):
    m = hashlib.md5()
    m.update(src)
    return m.hexdigest()


def md5str(src: str, encoding='utf-16'):
    return md5(src.encode(encoding))

