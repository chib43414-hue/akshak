#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 /dev/sdX-or-/dev/nvmeXn1"
  echo "This permanently erases the selected device. Do not use a partition path."
  exit 2
fi

DEVICE="$1"
IMAGE="$(pwd)/build/aurora-vault-os.iso"

if [[ ! -b "$DEVICE" ]]; then
  echo "Not a block device: $DEVICE" >&2
  exit 1
fi
if [[ "$DEVICE" =~ [0-9]$|p[0-9]+$ ]]; then
  echo "Refusing to write to a partition. Pass the whole USB device, such as /dev/sdb." >&2
  exit 1
fi
if [[ ! -f "$IMAGE" ]]; then
  echo "Image not found at $IMAGE. Run: cargo build" >&2
  exit 1
fi

echo "Selected device: $DEVICE"
lsblk -o NAME,SIZE,MODEL,TRAN,MOUNTPOINTS "$DEVICE"
echo
echo "WARNING: every partition on $DEVICE will be overwritten."
read -r -p "Type ERASE-$DEVICE to continue: " CONFIRM
if [[ "$CONFIRM" != "ERASE-$DEVICE" ]]; then
  echo "Cancelled."
  exit 1
fi

for mount in $(lsblk -nrpo MOUNTPOINT "$DEVICE" | sed '/^$/d'); do
  sudo umount "$mount" || true
done
sudo dd if="$IMAGE" of="$DEVICE" bs=4M status=progress conv=fsync
sync
echo "USB image written. Safely eject the device before removing it."
