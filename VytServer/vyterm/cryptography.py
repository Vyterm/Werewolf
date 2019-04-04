import hashlib
import struct
from functools import *
from string import ascii_lowercase, ascii_uppercase
from abc import abstractmethod


def md5(src: bytes):
    m = hashlib.md5()
    m.update(src)
    return m.hexdigest()


def md5str(src: str, encoding='utf-16'):
    return md5(src.encode(encoding))


class Cipher(object):
    @abstractmethod
    def Encrypt(self, original: bytes) -> bytes:
        pass

    @abstractmethod
    def Decrypt(self, original: bytes) -> bytes:
        pass


class ReplaceCipher(Cipher):
    def __init__(self, encrypt_table):
        self.__encrypt_table = encrypt_table
        self.__decrypt_table = {value: key for key, value in encrypt_table.items()}

    def Encrypt(self, original: str) -> str:
        return "".join([self.__encrypt_table[c] if c in self.__encrypt_table else c for c in original])

    def Decrypt(self, original: str) -> str:
        return "".join([self.__decrypt_table[c] if c in self.__decrypt_table else c for c in original])


class CaesarCipher(ReplaceCipher):
    # ascii_lowercase = "".join(map(chr, range(0x61, 0x7B)))
    # ascii_uppercase = "".join(map(chr, range(0x41, 0x5B)))

    def __init__(self, offset):
        offset = offset % 26
        tochar = lambda ac, of: chr(abs((ac - of) % 26) + of)
        asciioff = lambda c: 0x61 if 0x61 <= ord(c) <= 0x7A else 0x41 if 0x41 <= ord(c) <= 0x5A else 0
        turntable = lambda c: tochar(ord(c) + offset, asciioff(c))
        super().__init__({c: turntable(c) for c in ascii_lowercase + ascii_uppercase})


class IndexForBit:
    def __init__(self, offset_of_bit=0):
        self.offset_of_bit = offset_of_bit

    @property
    def byteIndex(self):
        return self.offset_of_bit // 8

    @property
    def bitIndex(self):
        return self.offset_of_bit % 8

    def __add__(self, other):
        return IndexForBit(self.offset_of_bit + other)

    def __iadd__(self, other):
        self.offset_of_bit += other
        return self

    def __str__(self):
        return "%d:%d" % (self.byteIndex, self.bitIndex)

_tolist = lambda i, a: list(map(chr, range(i, a)))
_SuggestKeys = _tolist(0x41, 0x5B)+_tolist(0x61, 0x7B)+_tolist(0x30, 0x3A)

class Base64(Cipher):
    BasicCodeTable = _tolist(0x41, 0x5B) + _tolist(0x61, 0x7B) + _tolist(0x30, 0x3A)
    __DefaultCodeTable = BasicCodeTable + [c for c in '+/']
    __EmptyBytes = b'='

    def __init__(self, code_table=None):
        if code_table is None:
            code_table = Base64.__DefaultCodeTable
        assert len(code_table) == 64
        self.code_table = code_table

    def Encrypt(self, original: bytes) -> bytes:
        cipher_text = bytes()
        i = IndexForBit()
        while i.byteIndex < len(original):
            if i.bitIndex == 0:
                t = original[i.byteIndex] >> 2
            else:
                t = (original[i.byteIndex] & 0xFF >> i.bitIndex) << i.bitIndex - 2
                if i.byteIndex + 1 < len(original):
                    t += original[i.byteIndex + 1] >> 10 - i.bitIndex  # 10-o == 8-(6+o-8)
            cipher_text += self.code_table[t].encode()
            i += 6
        return cipher_text

    def Decrypt(self, base64bytes: bytes) -> bytes:
        # origin = bytes(len(base64bytes)*3//4) # Python不支持对字节流按下标进行操作
        origin = [0 for _ in range(len(base64bytes) * 3 // 4)]
        bi = IndexForBit()
        for i in base64bytes:
            i = self.code_table.index(chr(i))
            if 0 == bi.bitIndex:
                origin[bi.byteIndex] = i << 2
            elif 6 == bi.bitIndex:
                origin[bi.byteIndex] += i >> 4
                if bi.byteIndex + 1 < len(origin):
                    origin[bi.byteIndex + 1] = (i & 0xFF >> 4) << 4
            elif 4 == bi.bitIndex:
                origin[bi.byteIndex] += i >> 2
                if bi.byteIndex + 1 < len(origin):
                    origin[bi.byteIndex + 1] = (i & 0xFF >> 6) << 6
            elif 2 == bi.bitIndex:
                origin[bi.byteIndex] += i
            bi += 6
        o = bytes()
        for b in origin:
            o += struct.pack('B', b)
        return o


if __name__ == '__main__':
    # origin = _SuggestKeys.copy()
    # cipher = origin.copy()
    # random.shuffle(cipher)
    # rc = ReplaceCipher({origin[i]: cipher[i] for i in range(len(origin))})
    # ct = rc.Encrypt("Vyterm123456")
    # print(ct)
    # print(rc.Decrypt(ct))
    # cc = [CaesarCipher(i) for i in range(26)]
    # src = "Vyterm"
    # print("Orginal String is: ", src)
    # des = cc[3].Encrypt(src)
    # print("Cipher text    is: ", des)
    # for i in range(1, 26):
    #     print("Plain text     is: ", cc[i].Decrypt(des))
    b64 = Base64()
    cipher = b64.Encrypt('哈哈哈哈哈'.encode())
    print(cipher.decode())
    print(b64.Decrypt(cipher).decode())
    cipher = b64.Encrypt('哈哈哈哈哈'.encode('utf-16')[2:])
    print(cipher.decode())
    print(b64.Decrypt(cipher).decode('utf-16'))
    # print(reduce(lambda l,r:l+r, Base64._Base64__DefaultCodeTable))
    pass
