#include "try.h"
#include "./base/heap.h"
#include "./base/logger.h"

int cmpHeapItem(const void* a, const void* b) {

    const HeapItem* x = (const HeapItem*)a;
    const HeapItem* y = (const HeapItem*)b;

    if (y->freq > x->freq) return 1;
    if (y->freq < x->freq) return -1;
    return 0;
}

char* autocomplete(TryNode* node, const i8* key, u32 K) {
    TryNode* root = try_walk_pref(node, key);

    if (!root) {
        char* out = malloc(64);
        snprintf(out, 64, "No suggestions");
        return out;
    }

    Heap* h = &root->_topK;

    if (h->count == 0) {
        char* out = malloc(64);
        snprintf(out, 64, "No suggestions");
        return out;
    }

    if (K > h->count)
        K = h->count;

    HeapItem* temp =
        malloc(sizeof(HeapItem) * h->count);

    memcpy(temp, h->items,
           sizeof(HeapItem) * h->count);

    qsort(temp, h->count,
          sizeof(HeapItem), cmpHeapItem);



    size_t buf_size = K * WORD;
    char* out = malloc(buf_size);

    if (!out) {
        free(temp);
        return NULL;
    }

    out[0] = '\0';

    char line[256];

    for (u32 i = 0; i < K; i++) {
        snprintf(
            line,
            sizeof(line),
            "%s (%u)\n",
            temp[i].word,
            temp[i].freq
        );

        strncat(out, line, buf_size - strlen(out) - 1);
    }

    free(temp);
    return out;
}

void load_data_from_file(const i8 *filename, TryNode *root) {
  FILE *fp = fopen((const i8 *)filename, "r");
  if (!fp) {
    perror("fopen");
    return;
  }
  char buf[256];
  while (fgets(buf, sizeof(buf), fp)) {

    buf[strcspn(buf, "\n")] = '\0';

    if (buf[0] == '\0')
      continue;
    if (strlen(buf) >= WORD)
      continue;

    try_insert_key(root, (i8 *)buf);
  }
  fclose(fp);
}

void try_save_node(FILE *fp, TryNode *node) {
  fwrite(&node->_freq, sizeof(u32), 1, fp);
  fwrite(&node->_prefix_count, sizeof(u32), 1, fp);
  fwrite(&node->_child_count, sizeof(u32), 1, fp);
  fwrite(&node->_end, sizeof(bool), 1, fp);


  u32 hcount = node->_topK.count;
  fwrite(&hcount, sizeof(u32), 1, fp);

  if (hcount > 0) {
    fwrite(node->_topK.items,
        sizeof(HeapItem),
        hcount,
        fp);
  }

  for (u32 i = 0; i < ASCII_LEN; i++) {

    if (node->_childrens[i]) {
      u8 idx = (u8)i;
      fwrite(&idx, sizeof(u8), 1, fp);
      try_save_node(fp, node->_childrens[i]);
    }
  }

  u8 end = 0xFF;
  fwrite(&end, sizeof(u8), 1, fp);
}


TryNode *trie_load_node(FILE *fp) {
  TryNode *node = try_create_node();

  fread(&node->_freq, sizeof(u32), 1, fp);
  fread(&node->_prefix_count, sizeof(u32), 1, fp);
  fread(&node->_child_count, sizeof(u32), 1, fp);
  fread(&node->_end, sizeof(bool), 1, fp);


  u32 hcount;
  fread(&hcount, sizeof(u32), 1, fp);

  heap_init(&node->_topK);

  for (u32 i = 0; i < hcount; i++) {
    HeapItem item;
    fread(&item, sizeof(HeapItem), 1, fp);
    heap_insert(&node->_topK, item);
  }

  while (1) {
    u8 idx;
    fread(&idx, sizeof(u8), 1, fp);

    if (idx == 0xFF)
      break;
    node->_childrens[idx] = trie_load_node(fp);
  }
  return node;
}


void try_save(const char *filename, TryNode *root) {
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    perror("fopen");
    return;
  }
  try_save_node(fp, root);
  fclose(fp);
}

TryNode *try_load(const char *filename) {
  FILE *fp = fopen(filename, "rb");

  if (!fp) {
    perror("fopen");
    return NULL;
  }

  TryNode *root = trie_load_node(fp);
  fclose(fp);
  return root;
}


int main(int argc, char* argv[]) {
  auto n = try_load("./corpuses/trycorpus.he");

  if (argc <= 2) {
    log_error("Error its an command line argument");
    return 1;
  }

  i8* s = argv[1];
  u32 _si = atoi(s);
  i8* s_ = argv[2];

  autocomplete(n, s_, _si);
  try_node_free(n);
  return 0;  
}

