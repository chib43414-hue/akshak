# Aurora Vault OS Architecture

## Purpose

Aurora Vault OS is a new operating-system project for x86_64 Dell Inspiron-class hardware. It is independent of the Linux kernel. The system prioritizes temporary sessions, explicit permissions, small trusted components, offline documentation, and user-visible privacy state.

## Layered design

| Layer | Responsibility | Release target |
|---|---|---|
| Firmware boundary | BIOS/UEFI handoff, memory map, boot verification | Bootable now; project-owned loader later |
| Kernel | CPU setup, protected memory, interrupts, scheduling, system calls | Next major milestone |
| Hardware drivers | Keyboard, mouse, framebuffer, storage, USB, network, audio, power | Added incrementally and tested on hardware |
| Core services | Device permissions, RAM session, encrypted persistence, time, logging | Full live release |
| Userland | Terminal, process launcher, manual viewer, settings, privacy dashboard | Full live release |
| Desktop shell | Lightweight compositor, panels, windows, eDEX-style terminal | Full live release |
| Communication layer | Isolated browser/service launchers, cookie cleanup, network indicator | After networking and process isolation |
| Assistant layer | Offline command help first; optional external AI only by explicit consent | After userland is stable |

## Boot and hardware target

The first supported target is ordinary Intel/AMD x86_64 hardware, including the user's Dell Inspiron with an Intel i3 processor. The current image uses a Multiboot2-compatible boot path to reach the kernel on BIOS and UEFI-compatible machines. A later release should add a project-owned UEFI loader, signed release artifacts, and a reproducible build process.

## Privacy architecture

The system starts with networking disabled and treats persistence as an opt-in capability. The session manager owns temporary storage and must clear its in-memory state during shutdown. A future persistence service will require an explicit user action, derive keys from a passphrase using a standard password-based key derivation function, and expose its mounted state in the privacy dashboard. Camera and microphone access will be mediated by permissions and visible indicators rather than hidden background access.

## Application strategy

A completely new kernel cannot immediately run Firefox, Brave, Discord, or other large applications because those applications depend on a mature userland, graphics stack, networking, TLS, filesystem, process model, and browser runtime. Aurora Vault therefore uses a staged strategy. The first full desktop release will include a lightweight built-in manual viewer and privacy settings. A later communication release will provide isolated browser/service launchers, with networking off until the user enables it. Native application compatibility is a separate long-term project and is not claimed by the current release.

## AI strategy

The first assistant is an offline command and manual search helper that does not transmit user data. If an external AI service is added later, it must be clearly marked as network-dependent, disabled by default, and guarded by a permission prompt. The OS build itself will not depend on API keys.

## Completion criteria for a full live release

Aurora Vault OS should not be called a full release until it can boot on the target Dell, provide a graphical desktop, accept keyboard and mouse input, manage processes with isolation, read and write an explicitly selected encrypted persistence area, control network state, display sensor permissions, open the offline manuals, recover from common driver failures, and shut down cleanly. Browser and communication support must be tested separately from the kernel and must not silently defeat the privacy defaults.

## References

[1]: https://uefi.org/specs/UEFI/2.10/01_Introduction.html "UEFI Specification 2.10 — Introduction"
[2]: https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html "GNU GRUB Multiboot2 documentation"
[3]: https://os.phil-opp.com/minimal-rust-kernel/ "Writing an OS in Rust — A Minimal Rust Kernel"
