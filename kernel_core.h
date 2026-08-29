#ifndef AURORA_KERNEL_CORE_H
#define AURORA_KERNEL_CORE_H

#include <stdint.h>

void kernel_core_init(void);
void timer_tick(void);
void syscall_dispatch(uint32_t number, uint32_t argument);
uint64_t kernel_ticks(void);

#endif
