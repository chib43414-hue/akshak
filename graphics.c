#include "graphics.h"
#include <stddef.h>

#define MULTIBOOT2_TAG_END 0
#define MULTIBOOT2_TAG_FRAMEBUFFER 8

struct tag_header { uint32_t type; uint32_t size; } __attribute__((packed));
struct framebuffer_tag {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint16_t reserved;
} __attribute__((packed));

static volatile uint8_t* fb;
static uint32_t pitch;
static uint32_t width;
static uint32_t height;
static uint8_t bpp;
static uint8_t available;

static void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (!available || x >= width || y >= height || bpp < 24) return;
    volatile uint8_t* pixel = fb + y * pitch + x * (bpp / 8);
    pixel[0] = (uint8_t)(color & 0xFF);
    pixel[1] = (uint8_t)((color >> 8) & 0xFF);
    pixel[2] = (uint8_t)((color >> 16) & 0xFF);
}

static void rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    if (!available) return;
    uint32_t end_x = x + w < width ? x + w : width;
    uint32_t end_y = y + h < height ? y + h : height;
    for (uint32_t py = y; py < end_y; py++) {
        for (uint32_t px = x; px < end_x; px++) put_pixel(px, py, color);
    }
}

static void border(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    rect(x, y, w, 2, color);
    rect(x, y + h - 2, w, 2, color);
    rect(x, y, 2, h, color);
    rect(x + w - 2, y, 2, h, color);
}

void graphics_init(uint32_t multiboot_info_address) {
    available = 0;
    if (multiboot_info_address == 0) return;
    uint32_t total_size = *(uint32_t*)(uintptr_t)multiboot_info_address;
    uint8_t* cursor = (uint8_t*)(uintptr_t)(multiboot_info_address + 8);
    uint8_t* end = (uint8_t*)(uintptr_t)(multiboot_info_address + total_size);

    while (cursor < end) {
        struct tag_header* header = (struct tag_header*)cursor;
        if (header->type == MULTIBOOT2_TAG_END) break;
        if (header->type == MULTIBOOT2_TAG_FRAMEBUFFER && header->size >= sizeof(struct framebuffer_tag)) {
            struct framebuffer_tag* tag = (struct framebuffer_tag*)cursor;
            fb = (volatile uint8_t*)(uintptr_t)tag->framebuffer_addr;
            pitch = tag->framebuffer_pitch;
            width = tag->framebuffer_width;
            height = tag->framebuffer_height;
            bpp = tag->framebuffer_bpp;
            available = tag->framebuffer_type == 1 && width > 0 && height > 0;
            break;
        }
        cursor += (header->size + 7) & ~7u;
    }

    if (!available) return;
    rect(0, 0, width, height, 0x07121C);
    rect(0, 0, width, 48, 0x102B3C);
    rect(0, 46, width, 2, 0xDCDCDC);

    uint32_t margin = width > 80 ? 40 : 8;
    uint32_t right = width > 900 ? 300 : width / 3;
    uint32_t main_w = width - margin * 2 - right - 24;
    uint32_t top = 82;
    uint32_t panel_h = height > 180 ? height - top - 32 : 100;
    rect(margin, top, main_w, panel_h, 0x0D2233);
    border(margin, top, main_w, panel_h, 0x46DCDC);
    rect(margin + 20, top + 72, main_w - 40, 2, 0x1C4555);
    rect(margin + 20, top + 110, main_w - 40, 2, 0x1C4555);
    rect(margin + 20, top + 148, main_w - 40, 2, 0x1C4555);
    rect(margin + 20, top + 186, main_w - 40, 2, 0x1C4555);
    rect(margin + 20, top + 224, main_w - 40, 2, 0x1C4555);

    uint32_t rx = margin + main_w + 24;
    rect(rx, top, right, panel_h, 0x0D2233);
    border(rx, top, right, panel_h, 0x46DCDC);
    rect(rx + 18, top + 64, right - 36, 2, 0x1C4555);
    rect(rx + 18, top + 122, right - 36, 2, 0x1C4555);
    rect(rx + 18, top + 180, right - 36, 2, 0x1C4555);
    rect(rx + 18, top + 238, right - 36, 2, 0x1C4555);
    rect(rx + 18, top + 296, right - 36, 2, 0x1C4555);
    rect(rx + 18, top + 22, 72, 6, 0x64F0A0);
}

uint8_t graphics_available(void) { return available; }
