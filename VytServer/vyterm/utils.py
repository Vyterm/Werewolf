import functools


def singleton(cls):
    """
    此装饰器用于类，作用为使该类在全局唯一，且初始化一次
    设置该类的 _INITIAL_FLAG_ 属性为任意值会在下次构造该类时重新初始化
    :param cls: 被装饰的类
    :return: 装饰后的类
    """
    _instance = {}

    old_new = cls.__new__
    old_init = cls.__init__

    @functools.wraps(old_new)
    def new_new(*args, **kwargs):
        if cls not in _instance:
            _instance[cls] = old_new(*args, **kwargs)
        return _instance[cls]

    @functools.wraps(old_init)
    def new_init(*args, **kwargs):
        if hasattr(cls, '_INITIAL_FLAG_'):
            old_init(*args, **kwargs)
            del cls._INITIAL_FLAG_

    cls._INITIAL_FLAG_ = None
    cls.__init__ = new_init
    cls.__new__ = new_new
    return cls


def get_instance(cls):
    cls._instance = None

    def get():
        if not cls._instance:
            cls._instance = cls()
        return cls._instance

    cls.get = get

    return cls
