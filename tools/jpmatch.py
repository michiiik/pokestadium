"""Shared US/JP instruction matching helpers."""
import array
import sys


def masked_word(w):
    """Zero the address-bearing fields of a MIPS instruction."""
    op = w >> 26
    if op == 0:
        return w
    if op in (2, 3):
        return op << 26
    return w & 0xFFFF0000


def mask_stream(buf):
    words = array.array("I")
    words.frombytes(buf[: len(buf) // 4 * 4])
    if sys.byteorder == "little":
        words.byteswap()
    return array.array("I", (masked_word(w) for w in words)).tobytes()


def unique_find(hay, needle, step=1):
    """Offset of the only occurrence of needle in hay, aligned to step, else None."""
    first = hay.find(needle)
    if first < 0:
        return None
    if hay.find(needle, first + 1) >= 0:
        return None
    if step > 1 and first % step:
        return None
    return first


def selftest():
    assert masked_word(0x3C1F8010) == 0x3C1F0000
    assert masked_word(0x27188010) == 0x27180000
    assert masked_word(0x0C123456) == 0x0C000000
    assert masked_word(0x08123456) == 0x08000000
    assert masked_word(0x0351D822) == 0x0351D822
    assert unique_find(b"__X__", b"X") == 2
    assert unique_find(b"XX", b"X") is None
    assert unique_find(b"__X_", b"X", step=4) is None
    return "ok"


if __name__ == "__main__":
    print("selftest", selftest())
