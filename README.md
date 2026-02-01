# c_autocomplete

A **minimal, high-performance autocomplete engine in C** using **Trie + Top-K MinHeap**.
Supports **frequency-ranked suggestions**, persistent storage, and fast lookups. Ideal for building lightweight autocomplete services, CLI tools, or integration into larger projects.

---

## Features

* **Trie-based prefix search** for O(len(prefix)) lookups.
* **Top-K suggestions per node** using a **min-heap** to keep the K most frequent words.
* **Frequency-aware ranking**: most frequently inserted words appear first.
* **Persistent storage**: save/load the trie and heaps to/from disk for fast startup.
* **Handles large corpora** with efficient memory usage.
* **Safe**: prevents duplicates in Top-K, avoids segfaults on missing prefixes.

---

## File Structure

```
c_autocomplete/
│
├─ base/
│   ├─ heap.h          # MinHeap implementation for Top-K
│   ├─ logger.h        # Simple debug logging
│
├─ try.h               # Trie Node definition + insert/search
├─ try.c               # Trie insert, walk, save/load
├─ main.c              # Example/test program
├─ words.txt           # Sample corpus (newline separated)
├─ README.md
```

---

## Installation / Build

1. Clone the repository:

```bash
git clone <repo_url>
cd c_autocomplete
```

2. Compile:

```bash
gcc main.c try.c -o c_autocomplete -Wall -Wextra
```

> Optional: add `-g` for debugging or `-O2` for optimization.

---

## Usage

### Load a corpus from file:

```c
TryNode *root = try_create_node();
load_data_from_file("words.txt", root);
```

### Insert a word:

```c
try_insert_key(root, "example");
```

### Autocomplete a prefix:

```c
autocomplete(root, "ex");
```

**Example Output:**

```
Suggestions for "ex":
example (15)
excellent (12)
exercise (8)
```

---

## Save / Load Trie

### Save to disk:

```c
try_save("trie.bin", root);
```

### Load from disk:

```c
TryNode *root = try_load("trie.bin");
```

This allows **fast startup** without rebuilding the trie from the corpus.

---

## Implementation Details

* **Trie Node**:

```c
struct TryNode {
    TryNode* _childrens[ASCII_LEN];
    u32 _freq;
    u32 _prefix_count;
    u32 _child_count;
    bool _end;
    Heap _topK;
};
```

* **Top-K MinHeap** ensures:

  * Max frequency words stay on top.
  * Duplicates are avoided.
  * Efficient insertion and extraction.

* **Persistent Storage**:

  * Binary save/load functions recursively store trie + heaps.
  * Fast reloads, no need to reprocess corpus.

---

## Logging

Debug output uses `log_debug()` defined in `logger.h`:

```c
log_debug("%s (%u)\n", word, freq);
```

---

## Future Improvements

* Expose as **HTTP API** for web/mobile clients.
* Add **fuzzy matching** (typo tolerance).
* Optimize Top-K with **hashmap for O(1) lookup**.
* Compress Trie using **Radix tree**.
* Multi-threaded insert / autocomplete.

---

## License

MIT License © 2026

---

## Author

Aditya Nath

