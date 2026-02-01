#ifndef HEAP_H
#define HEAP_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "types.h"


#define WORD 32
#define auto __auto_type
/***
 *
 *
 * this is the minimal implementaton of heap data structure
 * and we are using this only to have accopanied with our 
 * minimal autocomplete text engine which uses try and internally 
 * uses 64 words buffer so that we can have the thing such as 
 * auto complete the words till 64 characters or even 32
 */

/** Dynamic array semantics same as the tsoding  **/
#define da_append(xs, x)                                                             \
  do {                                                                             \
    if ((xs)->count >= (xs)->capacity) {                                         \
      if ((xs)->capacity == 0) (xs)->capacity = 256;                           \
      else (xs)->capacity *= 2;                                                \
      (xs)->items = realloc((xs)->items, (xs)->capacity*sizeof(*(xs)->items)); \
    }                                                                            \
    \
    (xs)->items[(xs)->count++] = (x);                                            \
  } while (0)


typedef struct {
  u32 freq;
  char word[WORD];
}HeapItem;

typedef struct {
  /** **/
  HeapItem* items;
  u32 count;
  u32 capacity;
}Heap;

DECL void heapifyDown(Heap* h, u32 i) {

  u32 smallest = i; 
  u32 left = 2*i + 1;
  u32 right = 2*i + 2;

  if (left < h->count &&
      h->items[left].freq < h->items[smallest].freq)
    smallest = left;

  if (right < h->count &&
      h->items[right].freq < h->items[smallest].freq)
    smallest = right;

  if (smallest != i) {
    HeapItem tmp = h->items[i];
    h->items[i] = h->items[smallest];
    h->items[smallest] = tmp;

    heapifyDown(h, smallest);
  }
}

DECL void heap_init(Heap* hp) {
  hp->count = 0;
  hp->capacity = 0;
  hp->items = NULL;
}

DECL void heapifyUP(Heap* h, u32 i) {
  while (i > 0) {
    u32 parent = (i - 1) / 2;
    if (h->items[parent].freq <= h->items[i].freq)
      break;
    
    auto tmp = h->items[i];
    h->items[i] = h->items[parent];
    h->items[parent] = tmp;

    i = parent;
  }
}

DECL void heap_insert(Heap* h, HeapItem val) {
  da_append(h, val);
  heapifyUP(h, h->count - 1);
}

DECL bool heap_empty(Heap* h) {
  return h->count == 0;
}

DECL void heap_free(Heap* h) {
  h->capacity = 0;
  h->count = 0;
  free(h->items);
  h->items = NULL;
}

DECL HeapItem extractMIN(Heap* h) {
  HeapItem i = {0};
  if (h->count == 0) return i;
  auto root = h->items[0];
  h->items[0] = h->items[h->count - 1];
  h->count--;
  heapifyDown(h, 0);
  return root;
}

#endif /** HEAP_H **/
