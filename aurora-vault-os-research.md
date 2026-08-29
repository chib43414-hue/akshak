# Aurora Vault OS — Initial Research Findings

## Boot and platform

The official UEFI Specification 2.10 describes UEFI as an interface between an operating system and platform firmware. It provides data tables plus boot and runtime service calls, creating a standard environment for booting an operating system. The specification is an interface definition; the OS developer decides how to use those structures and services.

Source: https://uefi.org/specs/UEFI/2.10/01_Introduction.html

The OSDev UEFI reference explains the practical distinction between legacy BIOS and UEFI. Legacy BIOS loads a 512-byte boot sector from the boot device and starts it in a legacy real-mode environment. UEFI loads a relocatable PE-format UEFI application from a FAT partition on a GPT or MBR device and passes it a system table containing platform information such as ACPI tables and the memory map. UEFI firmware initializes x86_64 systems into a modern long-mode environment, while a legacy path requires the bootloader to perform more CPU setup.

Source: https://wiki.osdev.org/UEFI

## Design implication

Aurora Vault OS should initially target x86_64 and use a UEFI loader, with an optional legacy BIOS compatibility path only after the first UEFI boot is stable. The first milestone should be a small independent kernel that prints its own terminal interface, receives a boot memory map, and runs from a USB image. Privacy features such as temporary storage, no telemetry, and network-off-by-default must be designed into the OS rather than added as an afterthought.

## Requirements gathered from the user

The user is currently developing from Arch Linux on an older Dell Inspiron with an Intel i3 processor. The desired OS is named Aurora Vault OS unless the user changes it. It should be a new OS rather than a Linux distribution; boot from USB; prioritize privacy; support temporary sessions; provide an eDEX-UI-inspired terminal and graphical environment; include manuals for ethical cybersecurity and digital privacy; optionally use a camera because no fingerprint sensor is present; offer an AI helper; and provide privacy-conscious access to Firefox, Brave, Twitter/X, Discord, and similar communication services.

## Important constraints

A camera cannot provide the same security guarantees as a hardware fingerprint reader or a hardware security key. Browser services and proprietary applications cannot be assumed to run natively in a brand-new kernel, so the initial design should provide browser-based communication access or carefully isolated compatibility components. A fully offline local AI assistant may be too large for an older i3 laptop, so the first version should use a lightweight local command/help system, with optional network AI only when the user explicitly enables it.

## References

[1]: https://uefi.org/specs/UEFI/2.10/01_Introduction.html "UEFI Specification 2.10 — Introduction"
[2]: https://wiki.osdev.org/UEFI "OSDev Wiki — UEFI"

## Architecture and implementation choice

The current bootloader crate documentation identifies an x86_64 bootloader that can create images for both legacy BIOS and UEFI systems, with separate BIOS and UEFI boot image builders. This is suitable for a first independent kernel while leaving room to replace or harden the boot path later.

Source: https://docs.rs/bootloader/latest/bootloader/

The Writing an OS in Rust guide demonstrates a freestanding 64-bit Rust kernel, a custom target, a `no_std` environment, a boot image, QEMU testing, and eventual real-machine testing. It explains that BIOS boot requires a small boot sector and CPU mode transitions, while UEFI offers a more modern firmware interface. The guide is useful as a development reference, but Aurora Vault OS will have its own kernel identity, shell, privacy policy, and userland rather than reproducing the guide's sample operating system.

Source: https://os.phil-opp.com/minimal-rust-kernel/

## Decision for the first release

Target standard Intel/AMD x86_64 hardware, with UEFI as the primary boot path and BIOS compatibility as a secondary test target. Use Rust for the kernel and low-level services because the project needs a freestanding environment and strong memory-safety support, while retaining small amounts of assembly only where CPU entry or interrupt setup requires it. Begin with a terminal-first kernel and a lightweight graphical shell after the boot path is stable. Use QEMU/OVMF for safe testing before any physical USB write.

## Privacy model

Aurora Vault OS will use an ephemeral-by-default session. The live system will keep working data in memory where practical, avoid telemetry and background analytics, keep networking disabled until explicitly enabled, and make persistence an explicit encrypted option rather than the default. The design will include a visible privacy status panel showing network state, storage mode, microphone/camera state, and whether any external AI service is enabled. This does not guarantee anonymity against compromised firmware, a malicious USB device, or a hostile network; those limits must be documented clearly.

## References

[3]: https://docs.rs/bootloader/latest/bootloader/ "bootloader crate documentation"
[4]: https://os.phil-opp.com/minimal-rust-kernel/ "Writing an OS in Rust — A Minimal Rust Kernel"
