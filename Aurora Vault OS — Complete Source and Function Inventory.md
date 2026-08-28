# Aurora Vault OS — Complete Source and Function Inventory

This document describes every source file in the current released stage. The project is deliberately small and freestanding: it does not link against a host operating-system runtime and it does not use the Linux kernel.

## Source tree

```text
 aurora-vault-os/
 ├── Makefile
 ├── README.md
 ├── boot.S
 ├── linker.ld
 ├── grub.cfg
 ├── kernel.c
 ├── kernel_core.h
 ├── kernel_core.c
 ├── memory.h
 ├── memory.c
 ├── privacy.h
 ├── privacy.c
 ├── paging.h
 ├── paging.c
 ├── graphics.h
 ├── graphics.c
 ├── ramfs.h
 ├── ramfs.c
 ├── docs/
 │   ├── architecture.md
 │   ├── build-verification.md
 │   ├── function-inventory.md
 │   ├── manual.md
 │   └── privacy-model.md
 ├── scripts/
 │   ├── install-deps-arch.sh
 │   └── write-usb.sh
 └── tools/
     └── ppm_to_png.py
```

## Boot and link files

| File | Functions |
|---|---|
| `boot.S` | Declares the Multiboot2 header, creates the kernel stack, enters `_start`, and defines `isr_timer` and `isr_syscall` interrupt stubs. |
| `linker.ld` | Places the kernel at the 1 MiB physical address, keeps the Multiboot2 header, and lays out text, read-only data, data, and BSS. |
| `grub.cfg` | Starts the single Aurora Vault ephemeral-session menu entry and loads `/boot/aurora.bin`. |
| `Makefile` | Compiles every object, links `build/kernel.bin`, validates the Multiboot2 header, creates `build/aurora-vault-os.iso`, runs QEMU, and cleans artifacts. |

## Kernel entry and terminal

`kernel.c` contains the first user-facing layer.

| Function | Purpose |
|---|---|
| `inb` | Reads one byte from an x86 I/O port. The keyboard driver uses ports `0x64` and `0x60`. |
| `halt_cpu` | Executes the x86 `hlt` instruction while waiting for input. |
| `put_cell` | Writes one character and color attribute to the VGA text buffer at `0xB8000`. |
| `clear_screen` | Clears the 80×25 text surface and resets the cursor. |
| `print_at` | Writes a null-terminated string at a selected VGA position. |
| `horizontal` | Draws a horizontal divider. |
| `draw_frame` | Draws the Aurora Vault terminal, privacy panel, kernel state, and status labels. |
| `same_command` | Compares a typed command with a built-in command name without using libc. |
| `show_command` | Dispatches `help`, `status`, `manual`, `privacy`, `about`, `manuals`, `ps`, `clear`, and unknown-command handling. |
| `key_to_ascii` | Converts common PS/2 set-1 keyboard scan codes into ASCII characters. The main loop also handles Shift and Caps Lock state. |
| `kmain` | Kernel entry point. It initializes graphics, paging, kernel services, memory, privacy, and RAMFS, then polls the keyboard. |

The current keyboard layer is intentionally conservative. It handles common PS/2 keys, Shift, Caps Lock, and backspace, but it does not yet include a full PS/2 controller driver, USB HID support, mouse input, or process-aware terminal sessions.

## Kernel core

`kernel_core.c` provides the first interrupt and syscall foundation.

| Function | Purpose |
|---|---|
| `outb` | Writes one byte to an x86 I/O port. |
| `lidt` | Loads the Interrupt Descriptor Table pointer. |
| `set_gate` | Places an interrupt or syscall handler address into one IDT entry. |
| `kernel_core_init` | Clears the IDT, installs timer and syscall gates, remaps the legacy PIC, programs the PIT at approximately 100 Hz, loads the IDT, and enables interrupts. |
| `timer_tick` | Increments the monotonic kernel tick counter and acknowledges the master PIC. |
| `kernel_ticks` | Returns the current tick counter. |
| `syscall_dispatch` | Controlled syscall placeholder. It intentionally performs no privileged action until process and capability checks exist. |
| `syscall_stub` | Assembly-callable wrapper for the syscall gate. |

`boot.S` calls `timer_tick` from `isr_timer` and `syscall_stub` from `isr_syscall`. The syscall interface is not a general application API yet.

## Memory

`memory.c` is the first bounded session heap.

| Function | Purpose |
|---|---|
| `memory_init` | Resets the heap cursor. |
| `memory_alloc` | Allocates aligned memory from a fixed 64 KiB heap and returns null on exhaustion. It does not free memory yet. |
| `memory_used` | Reports bytes consumed. |
| `memory_available` | Reports remaining heap capacity. |

The heap is not a complete virtual-memory manager. It is a temporary kernel arena used to establish explicit memory bounds before a real allocator and process isolation are implemented.

## Paging

`paging.c` provides the first x86 paging layer.

| Function | Purpose |
|---|---|
| `paging_init` | Creates an identity-mapped first 4 MiB page table, loads `CR3`, and sets the paging bit in `CR0`. |

The current map is intentionally simple and does not yet provide user/kernel separation, page-fault recovery, copy-on-write, or per-process address spaces. Those are required before a true multi-process userland can be called complete.

## Privacy service

`privacy.c` centralizes the default-deny capability policy.

| Function | Purpose |
|---|---|
| `privacy_init` | Sets network, camera, microphone, persistence, and external AI flags to disabled. |
| `privacy_state` | Returns a read-only pointer to the current in-memory policy state. |
| `privacy_set_network` | Changes network policy and disables external AI when networking is turned off. It does not yet drive hardware. |
| `privacy_set_camera` | Changes camera policy state. No camera driver exists in this stage. |
| `privacy_set_microphone` | Changes microphone policy state. No microphone driver exists in this stage. |
| `privacy_set_persistence` | Refuses to enable persistence until encrypted storage exists. |
| `privacy_set_external_ai` | Refuses external AI while networking is disabled. |

## Graphics

`graphics.c` searches the Multiboot2 information block for a framebuffer tag.

| Function | Purpose |
|---|---|
| `put_pixel` | Writes a 24-bit-or-greater framebuffer pixel. |
| `rect` | Fills a clipped rectangle. |
| `border` | Draws a rectangular border. |
| `graphics_init` | Finds a Multiboot2 framebuffer and renders a basic native dashboard background when available. |
| `graphics_available` | Reports whether a usable framebuffer was found. |

The current renderer is a dashboard background and panel system, not a full window compositor. The VGA terminal remains the reliable input surface for the current release.

## Process registry

`process.c` is the first process-service registry. It records up to eight service identities and gives each new entry the least-privilege default of no network or sensor capabilities.

| Function | Purpose |
|---|---|
| `process_init` | Clears all process slots and resets PID allocation. |
| `process_create` | Registers a named service with a PID, ready state, and optional kernel privilege flag. |
| `process_get` | Returns a read-only process entry by table index. |
| `process_count` | Counts active process entries. |

This is a registry, not yet a scheduler or isolated process runtime. It does not switch CPU contexts or create separate address spaces; those remain required for full userland security.

## PCI hardware discovery

`pci.c` reads x86 PCI configuration space through ports `0xCF8` and `0xCFC` and stores a bounded list of discovered devices.

| Function | Purpose |
|---|---|
| `config_read` | Reads one 32-bit PCI configuration register for a bus, slot, function, and offset. |
| `pci_scan` | Enumerates buses and slots, records vendor/device/class identifiers, and keeps up to 32 devices. |
| `pci_count` | Returns the number of devices recorded by the latest scan. |
| `pci_get` | Returns one read-only device record by index. |

The scanner does not yet bind drivers or enable any device. That deliberate separation prevents hardware access from bypassing the privacy policy.

## RAM-only userland catalog

`ramfs.c` provides built-in documents that disappear when power is removed.

| Function | Purpose |
|---|---|
| `ramfs_init` | Initializes the catalog; currently no dynamic work is needed. |
| `ramfs_read` | Returns the content of a known built-in path. |
| `ramfs_count` | Returns the number of built-in documents. |
| `ramfs_path` | Returns a path by index for the `manuals` terminal command. |

This is not a disk filesystem. It has no write operation, directory allocation, permissions, timestamps, or persistence.

## Host-side support

`scripts/install-deps-arch.sh` installs the Arch packages required to compile and test the image.

`scripts/write-usb.sh` validates that the requested target is a block device, rejects partition paths, shows the device, requires an exact confirmation, unmounts mounted filesystems, and writes the ISO with `dd`. It is destructive by design.

`tools/ppm_to_png.py` is only a development helper for converting a QEMU screenshot from PPM to PNG. It is not part of the booted OS.

## Current user-visible functions

| Command | Function |
|---|---|
| `help` | Lists safe commands. |
| `status` | Shows temporary session, network, storage, sensor, and telemetry state. |
| `manual` | Shows the safe cybersecurity learning categories. |
| `privacy` | Prints the authoritative default-deny capability state. |
| `about` | Shows the current release boundary and next engineering milestones. |
| `manuals` | Lists built-in RAM-only manual paths. |
| `ps` | Lists the current kernel service registry. |
| `devices` | Reports the number of PCI devices found during boot. |
| `clear` | Redraws the workspace. |

## Not implemented yet

The following are intentionally not claimed as working: PCI driver binding, native Firefox or Brave, Discord or Twitter/X clients, Wi-Fi, Ethernet, TCP/IP, TLS, browser rendering, camera or microphone capture, USB HID, mouse input, persistent storage, encrypted volumes, process isolation, a complete scheduler, audio, battery management, suspend/resume, a local language model, Secure Boot signing, and a project-owned UEFI loader.
