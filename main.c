#include "try.h"
#include "./base/heap.h"
#include "./base/logger.h"

void autocomplete(TryNode* node, const i8* key) {
  auto root = try_walk_pref(node,key);
  if (!root) {

    log_debug("No suggestions for '%s'\n", key);
    return;
  }
  Heap* h = &root->_topK;
  log_debug("suggestions: %d\n",node->_freq); 
  printf("%d\n",h->items[0].freq);
  for (u32 i = 0; i < h->count;i++) {
    log_debug("%s (%u)\n",
        h->items[i].word,
        h->items[i].freq);
  }
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


int main() {
  auto n = try_load("trycorpus.he");
  autocomplete(n, "sh");
  return 0;  
}

