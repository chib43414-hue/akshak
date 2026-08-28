#!/usr/bin/env bash
set -euo pipefail

sudo pacman -Syu --needed base-devel grub qemu-desktop edk2-ovmf xorriso
printf '\nAurora Vault OS build and test dependencies are ready.\n'
printf 'Next: make clean && make check && make iso\n'
