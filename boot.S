.set MULTIBOOT2_MAGIC, 0xe85250d6
.set MULTIBOOT_ARCHITECTURE_I386, 0
.set HEADER_LENGTH, (header_end - header_start)

.section .multiboot
.align 8
header_start:
.long MULTIBOOT2_MAGIC
.long MULTIBOOT_ARCHITECTURE_I386
.long HEADER_LENGTH
.long -(MULTIBOOT2_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + HEADER_LENGTH)
// Multiboot2 end tag.
.short 0
.short 0
.long 8
header_end:

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.type _start, @function
_start:
    cli
    mov $stack_top, %esp
    // Multiboot2 passes the magic value in eax and info address in ebx.
    push %ebx
    push %eax
    call kmain

.halt:
    hlt
    jmp .halt
.size _start, .-_start

.global isr_divide
.type isr_divide, @function
isr_divide:
    pusha
    pushl $0
    call exception_halt
    addl $4, %esp
    popa
    iret
.size isr_divide, .-isr_divide

.global isr_page_fault
.type isr_page_fault, @function
isr_page_fault:
    pusha
    pushl $14
    call exception_halt
    addl $4, %esp
    popa
    iret
.size isr_page_fault, .-isr_page_fault

.global isr_timer
.type isr_timer, @function
isr_timer:
    pusha
    call timer_tick
    popa
    iret
.size isr_timer, .-isr_timer

.global isr_syscall
.type isr_syscall, @function
isr_syscall:
    pusha
    call syscall_stub
    popa
    iret
.size isr_syscall, .-isr_syscall

.section .note.GNU-stack,"",@progbits
