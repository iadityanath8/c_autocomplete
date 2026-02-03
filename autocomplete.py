import ctypes
import os

LIB_PATH = os.path.abspath("./lib/libautocomplete.so")
libc = ctypes.CDLL("libc.so.6")
lib = ctypes.CDLL(LIB_PATH)
libc.free.argtypes = [ctypes.c_void_p]

TrieNodePtr = ctypes.c_void_p

lib.try_load.argtypes = [ctypes.c_char_p]
lib.try_load.restype = TrieNodePtr

lib.autocomplete.argtypes = [
    TrieNodePtr,
    ctypes.c_char_p,
    ctypes.c_uint32
]
lib.autocomplete.restype = ctypes.c_void_p

lib.try_node_free.argtypes = [TrieNodePtr]
lib.try_node_free.restype = None


TRIE = None
def init_tree(_path:str):
    global TRIE
    path = _path.encode()

    TRIE = lib.try_load(path)
    
    if not TRIE:
        raise RuntimeError("failed to load the ptr")
    
    print("trie loaded")

def free_trie():
    global TRIE 

    if TRIE:
        lib.try_node_free(TRIE)
        TRIE = False

def get_trie():
    global TRIE
    return TRIE

def parse_output(text: str):
    results = []

    for line in text.strip().split("\n"):
        if not line:
            continue

        try:
            word, rest = line.rsplit(" (", 1)
            freq = int(rest[:-1]) 

            results.append({
                "word": word,
                "freq": freq
            })

        except Exception:
            continue

    return results


def autocomplete(word: str, k: int = 5):
    res = lib.autocomplete(
        TRIE,
        word.encode(),
        k
    )

    if not res:
        return []

    text = ctypes.string_at(res).decode("utf-8")

    libc.free(res)
    return parse_output(text)



# if __name__ == "__main__":
#     init_tree("./lib/libautocomplete.so")