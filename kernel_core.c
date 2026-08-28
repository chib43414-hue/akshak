#include "kernel_core.h"
#include <stddef.h>

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t always_zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_pointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_pointer idt_ptr;
static volatile uint64_t ticks = 0;

extern void isr_timer(void);
extern void isr_syscall(void);
extern void isr_divide(void);
extern void isr_page_fault(void);

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline void lidt(const struct idt_pointer* pointer) {
    __asm__ volatile ("lidt (%0)" : : "r"(pointer));
}

static void set_gate(uint8_t number, uintptr_t handler, uint8_t flags) {
    idt[number].base_low = (uint16_t)(handler & 0xFFFF);
    idt[number].selector = 0x08;
    idt[number].always_zero = 0;
    idt[number].flags = flags;
    idt[number].base_high = (uint16_t)((handler >> 16) & 0xFFFF);
}

void kernel_core_init(void) {
    for (size_t i = 0; i < 256; i++) {
        idt[i].base_low = 0;
        idt[i].selector = 0;
        idt[i].always_zero = 0;
        idt[i].flags = 0;
        idt[i].base_high = 0;
    }

    // Interrupt gates: present, ring 0, 32-bit interrupt gate.
    set_gate(0, (uintptr_t)isr_divide, 0x8E);
    set_gate(14, (uintptr_t)isr_page_fault, 0x8E);
    set_gate(32, (uintptr_t)isr_timer, 0x8E);
    // User-callable syscall gate placeholder; policy checks will be added with processes.
    set_gate(128, (uintptr_t)isr_syscall, 0xEE);

    idt_ptr.limit = (uint16_t)(sizeof(idt) - 1);
    idt_ptr.base = (uint32_t)(uintptr_t)&idt;

    // Remap the legacy PIC away from CPU exception vectors 0..31.
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0xFE); outb(0xA1, 0xFF); // Enable only IRQ0 for now.

    // PIT channel 0: approximately 100 Hz.
    uint16_t divisor = 1193182 / 100;
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));

    lidt(&idt_ptr);
    __asm__ volatile ("sti");
}

void timer_tick(void) {
    ticks++;
    outb(0x20, 0x20); // End-of-interrupt to the master PIC.
}

uint64_t kernel_ticks(void) {
    return ticks;
}

void syscall_dispatch(uint32_t number, uint32_t argument) {
    (void)number;
    (void)argument;
    // Deliberately empty until process isolation and syscall capability checks exist.
}

void syscall_stub(void) {
    syscall_dispatch(0, 0);
}

__attribute__((noreturn)) void exception_halt(uint32_t vector) {
    volatile uint16_t* screen = (volatile uint16_t*)0xB8000;
    const char* label = vector == 0 ? "DIVIDE ERROR" : "PAGE FAULT";
    screen[0] = (uint16_t)('!' | (0x4F << 8));
    for (uint32_t i = 0; label[i] && i < 30; i++) screen[2 + i] = (uint16_t)(label[i] | (0x4F << 8));
    screen[40] = (uint16_t)('S' | (0x4F << 8));
    screen[41] = (uint16_t)('A' | (0x4F << 8));
    screen[42] = (uint16_t)('F' | (0x4F << 8));
    screen[43] = (uint16_t)('E' | (0x4F << 8));
    __asm__ volatile ("cli");
    for (;;) __asm__ volatile ("hlt");
}
