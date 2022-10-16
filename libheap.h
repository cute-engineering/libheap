#ifndef LIBHEAP_H
#define LIBHEAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define HEAP_MAGIC 0xc001c0dec001c0de
#define HEAP_DEAD 0xdeaddeaddeaddead
#define HEAP_ALIGN (64)
#define HEAP_PAGE_SIZE (4096)
#define HEAP_MIN_REQU (4096 * 4)
#define HEAP_ALIGNED(X) (((X) + (HEAP_ALIGN - 1)) & ~(HEAP_ALIGN - 1))
#define HEAP_PAGE_ALIGNED(X)                                                   \
  (((X) + (HEAP_PAGE_SIZE - 1)) & ~(HEAP_PAGE_SIZE - 1))

struct HeapNode {
  uint64_t magic;
  struct HeapNode *prev;
  struct HeapNode *next;
};

#define HEAP_NODE(T)                                                           \
  union {                                                                      \
    struct HeapNode base;                                                      \
    struct {                                                                   \
      uint64_t magic;                                                          \
      T *prev;                                                                 \
      T *next;                                                                 \
    };                                                                         \
  }

struct HeapMajor {
  HEAP_NODE(struct HeapMajor);
  size_t size;
  size_t used;
  struct HeapMinor *minor;
};

struct HeapMinor {
  HEAP_NODE(struct HeapMinor);
  size_t size;
  size_t used;
  struct HeapMajor *major;
};

typedef void *HeapAllocBlock(void *ctx, size_t size);

typedef void HeapFreeBlock(void *ctx, void *ptr, size_t size);

typedef void HeapPanic(void *ctx, const char *msg);

struct Heap {
  void *ctx;
  struct HeapMajor *root;
  struct HeapMajor *best;
  HeapAllocBlock *alloc;
  HeapFreeBlock *free;
  HeapPanic *panic;
};

// Heap hook functions

void *heap_alloc_block(struct Heap *heap, size_t size);

void heap_free_block(struct Heap *heap, void *ptr, size_t size);

void heap_panic(struct Heap *heap, const char *msg);

// Heap node functions

bool heap_node_check(struct HeapNode *node);

void heap_node_append(struct HeapNode *node, struct HeapNode *other);

void heap_node_prepend(struct HeapNode *node, struct HeapNode *other);

void heap_node_remove(struct HeapNode *node);

// Heap major functions

size_t heap_major_avail(struct HeapMajor *maj);

struct HeapMajor *heap_major_create(struct Heap *heap, size_t size);

struct HeapMinor *heap_major_alloc(struct HeapMajor *maj, size_t size);

void heap_major_free(struct Heap *heap, struct HeapMajor *maj);

struct HeapMinor *heap_major_alloc(struct HeapMajor *maj, size_t size);

// Heap minor functions

size_t heap_minor_avail(struct HeapMinor *min);

struct HeapMinor *heap_minor_create(struct HeapMajor *maj, size_t size);

struct HeapMinor *heap_minor_split(struct HeapMinor *min, size_t size);

void heap_minor_free(struct HeapMinor *min);

void heap_minor_resize(struct HeapMinor *min, size_t size);

struct HeapMinor *heap_minor_from(void *ptr);

void *heap_minor_to(struct HeapMinor *min);

// Heap functions

/*
TODO
void *heap_try_alloc(struct Heap *heap, size_t size);

void *heap_try_realloc(struct Heap *heap, void *ptr, size_t size);

void *heap_try_calloc(struct Heap *heap, size_t num, size_t size);
*/

void *heap_alloc(struct Heap *heap, size_t size);

void *heap_realloc(struct Heap *heap, void *ptr, size_t size);

void *heap_calloc(struct Heap *heap, size_t num, size_t size);

void heap_free(struct Heap *heap, void *ptr);

#endif
