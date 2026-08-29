#include "memory.h"
#include <stdint.h>

#define HEAP_SIZE (64 * 1024)
#define HEAP_ALIGNMENT 16

static uint8_t heap[HEAP_SIZE] __attribute__((aligned(HEAP_ALIGNMENT)));
static size_t used = 0;

void memory_init(void) {
    used = 0;
}

void* memory_alloc(size_t size) {
    if (size == 0) return (void*)0;
    size_t aligned = (size + HEAP_ALIGNMENT - 1) & ~(HEAP_ALIGNMENT - 1);
    if (aligned > HEAP_SIZE - used) return (void*)0;
    void* result = &heap[used];
    used += aligned;
    return result;
}

size_t memory_used(void) {
    return used;
}

size_t memory_available(void) {
    return HEAP_SIZE - used;
}
