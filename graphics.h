#ifndef AURORA_GRAPHICS_H
#define AURORA_GRAPHICS_H

#include <stdint.h>

void graphics_init(uint32_t multiboot_info_address);
uint8_t graphics_available(void);

#endif
