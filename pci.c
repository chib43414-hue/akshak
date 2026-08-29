#include "pci.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC
#define PCI_MAX_DEVICES 32

static PciDevice devices[PCI_MAX_DEVICES];
static size_t device_count = 0;

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline void outl(uint16_t port, uint32_t value) {
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t value;
    __asm__ volatile ("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static uint32_t config_read(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset) {
    uint32_t address = 0x80000000u
        | ((uint32_t)bus << 16)
        | ((uint32_t)slot << 11)
        | ((uint32_t)function << 8)
        | (offset & 0xFC);
    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

void pci_scan(void) {
    (void)outb;
    device_count = 0;
    for (uint16_t bus = 0; bus < 256 && device_count < PCI_MAX_DEVICES; bus++) {
        for (uint8_t slot = 0; slot < 32 && device_count < PCI_MAX_DEVICES; slot++) {
            uint32_t id = config_read((uint8_t)bus, slot, 0, 0);
            uint16_t vendor = (uint16_t)(id & 0xFFFF);
            if (vendor == 0xFFFF) continue;
            uint32_t class_data = config_read((uint8_t)bus, slot, 0, 8);
            devices[device_count++] = (PciDevice){
                .bus = (uint8_t)bus,
                .slot = slot,
                .function = 0,
                .class_code = (uint8_t)(class_data >> 24),
                .subclass = (uint8_t)(class_data >> 16),
                .prog_if = (uint8_t)(class_data >> 8),
                .vendor_id = vendor,
                .device_id = (uint16_t)(id >> 16),
            };
        }
    }
}

size_t pci_count(void) { return device_count; }
const PciDevice* pci_get(size_t index) {
    if (index >= device_count) return (const PciDevice*)0;
    return &devices[index];
}
