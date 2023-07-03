# JudgeDuck-OS-64-GRUB
JudgeDuck-OS Targeting x86-64 based on GRUB (both legacy BIOS and UEFI)

## Build and Run

Tested on `Ubuntu 20.04` and `gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)`.

Prerequisites:

```bash
sudo apt install gcc g++ gcc-multilib g++-multilib qemu-system-x86 grub-pc-bin xorriso nasm make grub-efi-amd64-bin ovmf

# Download duck-binaries repository at the parent directory
git clone https://github.com/JudgeDuck/duck-binaries.git ../duck-binaries
```

Build:

```bash
make
```

Run:

```bash
make run-legacy      # Run with QEMU (legacy BIOS)
make run-legacy-nox  # Run with QEMU (legacy BIOS, no-graphic)
make run-uefi        # Run with QEMU (UEFI)
make run-uefi-nox    # Run with QEMU (UEFI, no-graphic)
```

## Run on Real Machine

TODO
