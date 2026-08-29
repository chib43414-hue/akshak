#ifndef AURORA_PCI_H
#define AURORA_PCI_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t bus;
    uint8_t slot;
    uint8_t function;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t prog_if;
    uint16_t vendor_id;
    uint16_t device_id;
} PciDevice;

void pci_scan(void);
size_t pci_count(void);
const PciDevice* pci_get(size_t index);

#endif
