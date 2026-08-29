# Aurora Vault OS Operator Guide

## Important status statement

This guide covers the current working kernel release. It is a real bootable operating-system image, but it is not yet a complete daily-use desktop. The current release has its own freestanding kernel, boot handoff, VGA terminal, early paging, timer interrupts, bounded RAM heap, default-deny privacy service, framebuffer discovery, and RAM-only manual catalog. Networking, Wi-Fi, persistent storage, browsers, Discord, Twitter/X, camera access, microphone access, and AI are not yet implemented.

## 1. Install build tools on Arch Linux

Open a terminal in Arch Linux and enter:

```sh
cd /path/to/aurora-vault-os
./scripts/install-deps-arch.sh
```

The script installs the GNU build tools, GRUB utilities, QEMU, OVMF firmware, and ISO creation tools. You can inspect the script before running it.

## 2. Build everything

Run:

```sh
make clean
make check
make iso
```

The build performs these actions:

| Step | Output |
|---|---|
| Assemble `boot.S` | `build/boot.o` |
| Compile kernel modules | `build/kernel.o`, `kernel_core.o`, `memory.o`, `privacy.o`, `paging.o`, `graphics.o`, `ramfs.o` |
| Link the freestanding kernel | `build/kernel.bin` |
| Validate Multiboot2 | `grub-file` check |
| Assemble the boot image | `build/aurora-vault-os.iso` |

A successful build ends with `Created build/aurora-vault-os.iso`.

## 3. Test in QEMU first

Never begin with a valuable physical disk. Test the BIOS-compatible image in QEMU:

```sh
make run
```

A GRUB entry named **Aurora Vault OS - Ephemeral Session** should load. After the kernel starts, the terminal shows the privacy state and a prompt similar to:

```text
vault@aurora:~$
```

Try these commands:

```text
help
status
privacy
manual
manuals
about
clear
```

QEMU can be closed using its window close action or by returning to the terminal if the emulator was launched in a terminal display mode.

## 4. Write the image to a USB drive

Writing the image destroys the existing contents of the selected device. First list storage devices:

```sh
lsblk -o NAME,SIZE,MODEL,TRAN,MOUNTPOINTS
```

Identify the whole USB device, such as `/dev/sdb`. Do not select `/dev/sdb1`; that is only a partition. Then run:

```sh
./scripts/write-usb.sh /dev/sdb
```

Replace `/dev/sdb` with your actual USB device. The script displays the device and requires you to type an exact confirmation such as:

```text
ERASE-/dev/sdb
```

If you are not completely certain which device is the USB drive, cancel. Do not guess.

## 5. Boot the Dell Inspiron

Insert the USB drive while the Dell is powered off. Turn it on and repeatedly press the Dell boot-menu key, commonly `F12`. Select the USB storage device. If it does not appear, enter firmware setup, check whether USB boot is enabled, and try both legacy BIOS and UEFI boot options if the firmware offers them.

Keep the original operating system disk untouched until Aurora Vault has been tested successfully. Use the live image only for authorized defensive learning and privacy experimentation.

## 6. Troubleshooting

| Symptom | Action |
|---|---|
| `gcc: command not found` | Run `./scripts/install-deps-arch.sh`, or install `base-devel`. |
| `grub-file` is missing | Install the Arch `grub` package. |
| `grub-mkrescue` is missing | Install `grub` and `xorriso`. |
| QEMU does not start | Confirm `qemu-desktop` is installed. |
| USB is not shown in Dell boot menu | Try another USB port, verify the image write, and check firmware USB-boot settings. |
| Screen is blank on physical hardware | Try the legacy VGA boot option; framebuffer support is still early. |
| Wi-Fi does not work | This is expected in the current release; no network driver or TCP/IP stack exists yet. |
| Browser or Discord does not open | This is expected; application and browser layers are future milestones. |
| Build fails after source changes | Run `make clean` and rebuild, then read the first compiler error rather than the final summary. |

## 7. Privacy operating rules

The live release has no disk driver or filesystem, so it cannot save documents or credentials. This provides temporary behavior, but it is not a complete sanitization guarantee. Do not assume that firmware, hardware, another operating system, a malicious peripheral, or a hostile network is harmless. Do not enable or add networking components until their privacy policy and visible permission controls are in place.

## 8. Next release sequence

The next engineering sequence is a complete interrupt and exception table, page-fault handling, user/kernel address-space separation, process contexts, a real memory allocator, PS/2 and USB input, storage discovery, encrypted persistence, a graphical compositor, network device support, a small TCP/IP and TLS stack, isolated browser access, an offline manual search assistant, and hardware-specific testing on the Dell Inspiron. Each feature must have a build test and a privacy review before being called working.
