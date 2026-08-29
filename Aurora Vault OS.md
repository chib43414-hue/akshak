# Aurora Vault OS

Aurora Vault OS is an independent operating-system project for x86 PCs. It is **not a Linux distribution** and does not use the Linux kernel. The first milestone is a small freestanding kernel that boots from a USB-compatible ISO, displays a privacy dashboard, and accepts a few terminal commands.

## Current milestone

The current stage boots through a Multiboot2-compatible GRUB loader into a 32-bit kernel. It draws an eDEX-UI-inspired text workspace in VGA text mode and provides the commands `help`, `status`, `manual`, `privacy`, and `clear`. It now includes the first interrupt/timer foundation, a bounded kernel heap, and a default-deny privacy state service. The live design is ephemeral by intent: it has no disk driver, no filesystem, no telemetry, no network stack, and no microphone or camera access.

This is a **kernel prototype**, not yet a complete daily-use desktop operating system. Firefox, Brave, Discord, Twitter/X, and similar services will be added later through a deliberately isolated userland or browser compatibility layer. They cannot run natively until the OS has processes, memory protection, drivers, a filesystem, graphics, networking, and a browser runtime.

## Privacy design

Aurora Vault OS follows these initial rules:

| Rule | Current status |
|---|---|
| No telemetry or analytics | Enforced in the current stage |
| Network disabled by default | Enforced in the current stage |
| Session temporary by default | Enforced in the current stage; no storage exists yet |
| Kernel heap bounded to session memory | Enforced in the current stage |
| Capability state default-deny | Enforced in the current stage |
| Camera and microphone off by default | Enforced in the prototype; drivers do not exist yet |
| Persistence must be explicit and encrypted | Planned |
| AI assistant local by default | Planned; first version will be a small offline help system |
| Security manuals included offline | Planned in the next milestone |

No operating system can honestly guarantee absolute privacy. Firmware, hardware implants, malicious peripherals, radio leakage, physical observation, or a compromised network can bypass software-only protections. Aurora Vault OS will publish its threat model and audit assumptions as the project grows.

## Documentation

For exact commands, USB safety steps, boot-menu guidance, troubleshooting, and the current release boundary, read [`docs/operator-guide.md`](docs/operator-guide.md). For a file-by-file explanation of every function, read [`docs/function-inventory.md`](docs/function-inventory.md). The design rationale is in [`docs/architecture.md`](docs/architecture.md), and the privacy guarantees and limits are in [`docs/privacy-model.md`](docs/privacy-model.md).

## Build on Arch Linux

Install the required packages:

```sh
sudo pacman -S --needed base-devel grub qemu-desktop edk2-ovmf xorriso
```

Build and verify the kernel and ISO:

```sh
make clean
make check
make iso
```

Test safely in QEMU before touching any physical device:

```sh
make run
```

The resulting image is `build/aurora-vault-os.iso`. The ISO is intended to be written to a USB device only after it has been tested in QEMU.

## Write to USB

First identify the whole USB device, not a partition:

```sh
lsblk -o NAME,SIZE,MODEL,TRAN,MOUNTPOINTS
```

Then run the guarded script from this project directory:

```sh
./scripts/write-usb.sh /dev/sdX
```

Replace `/dev/sdX` only with the correct USB device. The script refuses partition paths and requires an exact confirmation before erasing the selected device. Writing an image permanently destroys existing data on that device.

## Roadmap

The current build already contains early paging, timer interrupts, a bounded session heap, a default-deny capability service, framebuffer discovery, a RAM-only manual catalog, and an expanded terminal. The next engineering milestones are a complete memory-protection model, exception handling, process contexts, a real terminal input layer, USB and storage drivers, an encrypted explicit-persistence option, a full manual viewer, and an isolated networking subsystem. Only after those foundations are stable should the project attempt a browser or communication application layer.

## References

[1]: https://uefi.org/specs/UEFI/2.10/01_Introduction.html "UEFI Specification 2.10 — Introduction"
[2]: https://docs.rs/bootloader/latest/bootloader/ "bootloader crate documentation"
[3]: https://github.com/rust-osdev/bootloader/blob/main/docs/create-disk-image.md "Rust OSDev bootloader — disk image template"
[4]: https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html "GNU GRUB Multiboot2 documentation"
