PROJECT := aurora-vault-os
BUILD := build
ISO_ROOT := $(BUILD)/iso
KERNEL := $(BUILD)/kernel.bin
ISO := $(BUILD)/$(PROJECT).iso

CC := gcc
LD := ld
CFLAGS := -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -Wall -Wextra -Werror -O2
ASFLAGS := -m32
LDFLAGS := -m elf_i386 -T linker.ld -nostdlib

.PHONY: all clean iso run run-uefi check

all: iso

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: boot.S | $(BUILD)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/kernel.o: kernel.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel_core.o: kernel_core.c kernel_core.h | $(BUILD)
	$(CC) $(CFLAGS) -c kernel_core.c -o $@

$(BUILD)/memory.o: memory.c memory.h | $(BUILD)
	$(CC) $(CFLAGS) -c memory.c -o $@

$(BUILD)/privacy.o: privacy.c privacy.h | $(BUILD)
	$(CC) $(CFLAGS) -c privacy.c -o $@

$(BUILD)/paging.o: paging.c paging.h | $(BUILD)
	$(CC) $(CFLAGS) -c paging.c -o $@

$(BUILD)/graphics.o: graphics.c graphics.h | $(BUILD)
	$(CC) $(CFLAGS) -c graphics.c -o $@

$(BUILD)/ramfs.o: ramfs.c ramfs.h | $(BUILD)
	$(CC) $(CFLAGS) -c ramfs.c -o $@

$(BUILD)/process.o: process.c process.h | $(BUILD)
	$(CC) $(CFLAGS) -c process.c -o $@

$(BUILD)/pci.o: pci.c pci.h | $(BUILD)
	$(CC) $(CFLAGS) -c pci.c -o $@

$(KERNEL): $(BUILD)/boot.o $(BUILD)/kernel.o $(BUILD)/kernel_core.o $(BUILD)/memory.o $(BUILD)/privacy.o $(BUILD)/paging.o $(BUILD)/graphics.o $(BUILD)/ramfs.o $(BUILD)/process.o $(BUILD)/pci.o linker.ld
	$(LD) $(LDFLAGS) -o $@ $(BUILD)/boot.o $(BUILD)/kernel.o $(BUILD)/kernel_core.o $(BUILD)/memory.o $(BUILD)/privacy.o $(BUILD)/paging.o $(BUILD)/graphics.o $(BUILD)/ramfs.o $(BUILD)/process.o $(BUILD)/pci.o
	grub-file --is-x86-multiboot2 $@

$(ISO_ROOT)/boot/grub/grub.cfg: grub.cfg $(KERNEL)
	mkdir -p $(ISO_ROOT)/boot/grub
	cp grub.cfg $(ISO_ROOT)/boot/grub/grub.cfg
	cp $(KERNEL) $(ISO_ROOT)/boot/aurora.bin

$(ISO): $(ISO_ROOT)/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(ISO_ROOT) >/dev/null

iso: $(ISO)
	@echo "Created $(ISO)"

check: $(KERNEL)
	@file $(KERNEL)
	@grub-file --is-x86-multiboot2 $(KERNEL) && echo "Multiboot2 header: OK"

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -m 256M

OVMF_CODE ?= /usr/share/edk2/x64/OVMF_CODE.fd
OVMF_VARS ?= /usr/share/edk2/x64/OVMF_VARS.fd

run-uefi: $(ISO)
	cp $(OVMF_VARS) $(BUILD)/OVMF_VARS.fd
	qemu-system-x86_64 -drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) -drive if=pflash,format=raw,file=$(BUILD)/OVMF_VARS.fd -cdrom $(ISO) -m 512M

clean:
	rm -rf $(BUILD)
