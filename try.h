#ifndef  TRY_H 
#define  TRY_H 
#define  ASCII_LEN 256
#include "./base/types.h"
#include "./base/heap.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
typedef struct TryNode TryNode;

#define auto __auto_type

struct TryNode {
  TryNode* _childrens[ASCII_LEN];
  u32      _freq;
  u32      _prefix_count;
  u32      _child_count;

  /** TOPK heap for topk per node optimization **/
  Heap     _topK; 
  bool     _end;
};

DECL void updateTopK(Heap *h, const i8 *word, u32 freq, u32 TOP_K) {
  /** right now linear scan **/
  for (u32 i = 0; i < h->count; i++) {

    if (strcmp(h->items[i].word, word) == 0) {

      h->items[i].freq = freq;

      heapifyDown(h, i);
      heapifyUP(h, i);

      return;
    }
  }

  HeapItem item;

  item.freq = freq;
  strncpy(item.word, word, WORD-1);
  item.word[WORD-1] = '\0';

  if (h->count < TOP_K) {
    heap_insert(h, item);
    return;
  }

  if (freq > h->items[0].freq) {
    extractMIN(h);
    heap_insert(h, item);
  }
}

DECL TryNode* try_create_node() {
  auto n = (TryNode*)malloc(sizeof(TryNode));  
  n->_child_count = 0;
  n->_freq = 0;
  n->_prefix_count = 0;
  n->_end = false;
  memset(n->_childrens, 0, ASCII_LEN * sizeof(TryNode*));
  heap_init(&n->_topK);
  return n;
}

DECL void try_insert_key(TryNode* root, const i8* key) {
  auto node = root;
  for (u32 i = 0; key[i] != '\0'; i++) {
    u32 idx = (u32)key[i];
    if (node->_childrens[idx] == NULL) {
      node->_childrens[idx] = try_create_node();
      node->_child_count++;
    }
    node = node->_childrens[idx];
    node->_prefix_count++;
  }

  node->_freq++;
  node->_end = true;
  u32 word_freq = node->_freq;  // Get the actual frequency

  node = root;
  for (u32 i = 0; key[i] != '\0'; i++) {
    node = node->_childrens[(u32)key[i]];
    updateTopK(&node->_topK, key, word_freq, 50);
  }
}

DECL bool try_search_key(TryNode* n, const i8* key) {
  for (u32 i = 0;key[i] != '\0';i++) {
    if (n->_childrens[(u32)key[i] ]== NULL) 
      return false;
    n = n->_childrens[(u32)key[i]];
  }
  return n->_end; 
}

DECL TryNode* try_walk_pref(TryNode* n, const i8* key) {
  TryNode* root = n;
  for (u32 i = 0;key[i] != '\0';i++) {
    if (!root->_childrens[(u32)key[i]]) {
      return NULL;
    }
    root = root->_childrens[(u32)key[i]];
  }
  return root;
}

DECL void try_node_free(TryNode* node) {

  if (!node) return;
  for (u32 i = 0; i < ASCII_LEN; i++) {
    if (node->_childrens[i]) {
      try_node_free(node->_childrens[i]);
    }
  }
  heap_free(&node->_topK);
  free(node);
}


#endif /** TRY_H **/