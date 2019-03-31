# import threading
# import gc
# import time
#
#
# class SampleClass:
#     def __del__(self):
#         print(self, " has Del")
#
#
# Quit = False
#
#
# # 测试析构是否被调用
# def CreateSample():
#     sc = SampleClass()
#     while not Quit:
#         # print(sc)
#         continue
#     print(sc)
#
#
# if __name__ == '__main__':
#     msc = SampleClass()
#     threading.Thread(name="Create Thread", target=CreateSample).start()
#     Quit = True
#
#     # time.sleep(1)
#     print(msc, "\nExit Done")
#
#
# # if __name__ == '__main__':
# #     msc = SampleClass()
# #     createThread = threading.Thread(name="Create Thread", target=CreateSample)
# #     # 设置父线程，让子线程伴随父线程退出
# #     createThread.setDaemon(True)
# #     createThread.start()
# #     del createThread
# #     Quit = True
# #
# #     print(msc, "\nExit Done")
# #
# #     time.sleep(1)
# #     gc.collect()
# #     exit(0)
#
# """
# 问题：
#     1. 子线程中创建的对象，在线程退出时如何让其自动调用析构函数
#     2. 当创建多线程的时候，类的析构就没有调用，为什么？
# """

# import struct
#
# if __name__ == '__main__':
#     print("Default:")
#     for i in struct.pack('=Bifd', True, 123456, 3, 3.14):
#         print(i, end=' ')
#     print("\nB:")
#     for i in struct.pack('B', True):
#         print(i, end=' ')
#     print("\ni:")
#     for i in struct.pack('i', 123456):
#         print(i, end=' ')
#     print("\nf:")
#     for i in struct.pack('f', 3):
#         print(i, end=' ')
#     print("\nd:")
#     for i in struct.pack('d', 3.14):
#         print(i, end=' ')


if __name__ == '__main__':
    # print('123456789'.isdigit())
    ia = {x: x for x in range(20)}
    for i in ia:
        if i % 2:
            ia.pop(i)

    print(ia)


