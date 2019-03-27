import struct


def string_to_bytes(src):
    src = src.encode('utf-16')[2:]
    return struct.pack('i', len(src)) + src


def bytes_to_string(src):
    return bytes_to_string_with_size(src)[0]


def bytes_to_string_with_size(src):
    size, = struct.unpack('i', src[:4])
    return src[4:size+4].decode('utf-16'), size+4


def strings_to_bytes(*sources):
    result = bytes()
    for src in sources:
        result += string_to_bytes(src)
    return result


def bytes_to_strings(sources):
    result = []
    index = 0
    while 0 <= index < len(sources):
        src, size = bytes_to_string_with_size(sources[index:])
        result.append(src)
        index += size
    return tuple(result)


# if __name__ == '__main__':
#     username = "Vyterm"
#     password = "123456"
#     bString = string_to_bytes(username)
#     assert bytes_to_string(bString) == username
#     print(bString)
#     bString = strings_to_bytes(username, password)
#     u, p = bytes_to_strings(bString)
#     assert u == username
#     assert p == password
#     print(bString)
#
#     print("Test Vyterm.Conversion Done, All tests passed")


