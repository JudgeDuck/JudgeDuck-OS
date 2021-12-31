# JudgeDuck-OS-64-GRUB
JudgeDuck-OS Targeting x86-64 based on GRUB

## Build and Run

Tested on `Ubuntu 20.04` and `gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)`.

Prerequisites:

```bash
sudo apt install gcc g++ gcc-multilib g++-multilib qemu-system-x86 grub-pc xorriso nasm make

# Download duck-binaries repository at the parent directory
git clone https://github.com/JudgeDuck/duck-binaries.git ../duck-binaries
```

Build:

```bash
make
```

Run:

```bash
make run      # Run with QEMU
make run-nox  # Run with QEMU (no-graphic)
```

## Run on Real Machine

TODO
