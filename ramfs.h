#ifndef AURORA_RAMFS_H
#define AURORA_RAMFS_H

#include <stddef.h>

void ramfs_init(void);
const char* ramfs_read(const char* path);
size_t ramfs_count(void);
const char* ramfs_path(size_t index);

#endif
