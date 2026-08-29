# Aurora Vault OS Build Verification

## Result

The prototype was built successfully on an x86_64 Ubuntu build environment using the same GNU toolchain available on Arch Linux. The kernel is a statically linked 32-bit Intel executable with a valid Multiboot2 header, and `grub-mkrescue` produced a bootable ISO image.

| Check | Result |
|---|---|
| Freestanding kernel compilation | Passed |
| Kernel interrupt/timer foundation | Passed; guest remained alive in QEMU |
| Identity-mapped paging foundation | Passed; guest remained alive in QEMU |
| Default-deny privacy service | Passed; linked into kernel |
| Linker output | Passed |
| Multiboot2 header validation | Passed |
| ISO generation | Passed |
| ISO type | ISO 9660, bootable, DOS/MBR boot sector |
| ISO size | Approximately 12 MiB |
| BIOS-mode QEMU boot | Passed; terminal dashboard displayed |
| UEFI-mode QEMU launch | Passed; OVMF accepted the ISO and remained running through the test window |
| Physical USB write | Not performed; it requires the user's explicit device selection |

## Reproduction

From the project directory:

```sh
make clean
make check
make iso
make run
```

The ISO is at `build/aurora-vault-os.iso`. Test it in QEMU before writing it to a USB device. The guarded writer is:

```sh
./scripts/write-usb.sh /dev/sdX
```

The placeholder must be replaced with the whole USB device, not a partition. The script lists the selected device, requires an exact confirmation string, unmounts its mounted filesystems, and then performs the destructive write.

## Technical note

The current stage uses a Multiboot2-compatible loader for the initial boot path so the project can reach real hardware quickly while keeping the kernel independent of Linux. It now includes early paging, timer interrupts, a bounded session heap, and a default-deny capability policy. The long-term plan is to replace or supplement this path with a project-owned UEFI loader and a signed, reproducible release process.

## References

[1]: https://uefi.org/specs/UEFI/2.10/01_Introduction.html "UEFI Specification 2.10 — Introduction"
[2]: https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html "GNU GRUB Multiboot2 documentation"
[3]: https://docs.rs/bootloader/latest/bootloader/ "bootloader crate documentation"
