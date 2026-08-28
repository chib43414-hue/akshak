#ifndef AURORA_MEMORY_H
#define AURORA_MEMORY_H

#include <stddef.h>

void memory_init(void);
void* memory_alloc(size_t size);
size_t memory_used(void);
size_t memory_available(void);

#endif
