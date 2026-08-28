#include "paging.h"
#include <stdint.h>

static uint32_t page_directory[1024] __attribute__((aligned(4096)));


void paging_init(void) {
    // Identity map the first 4GB using 4MB pages to avoid page faults on framebuffer/PCI MMIO.
    for (uint32_t i = 0; i < 1024; i++) {
        // 0x83 = Present (1) + Read/Write (2) + Page Size 4MB (128)
        page_directory[i] = (i * 0x400000) | 0x83;
    }

    // Enable Page Size Extensions (PSE) in CR4
    uint32_t cr4;
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= 0x00000010; // Set PSE bit (bit 4)
    __asm__ volatile ("mov %0, %%cr4" : : "r"(cr4) : "memory");

    __asm__ volatile ("mov %0, %%cr3" : : "r"(page_directory) : "memory");

    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000u; // Enable paging (PG bit)
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0) : "memory");
}
