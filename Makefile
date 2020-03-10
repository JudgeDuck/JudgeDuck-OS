kernel := build/kernel.bin
iso := build/os.iso

CXX := g++
CXX += -std=c++14
CXX += -Wall -Wextra -Werror -march=x86-64
CXX += -mno-red-zone
CXX += -U_FORTIFY_SOURCE
CXX += -I .

linker_script := boot/linker.ld
grub_cfg := boot/grub.cfg
assembly_source_files := $(wildcard boot/*.asm)
assembly_object_files := $(patsubst boot/%.asm, \
		build/boot/%.o, $(assembly_source_files))

header_files := $(wildcard inc/*.h)
libc_duck64 := ../libc-duck-64/lib/libc.a

include kern/Makefile
include lib/Makefile

.PHONY: all clean run iso

all: $(kernel)

clean:
	@rm -r build/*

QEMUOPTS ?= 

run: $(iso)
	@qemu-system-x86_64 -cdrom $(iso) -cpu Haswell $(QEMUOPTS)

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub-mkrescue -o $(iso) build/isofiles -d /usr/lib/grub/i386-pc 2> /dev/null
	@rm -r build/isofiles

$(kernel): $(assembly_object_files) $(kern_object_files) $(lib_object_files) $(linker_script) $(libc_duck64)
	@echo + ld $(kernel)
	@ld -n -T $(linker_script) -o $(kernel) $(assembly_object_files) $(kern_object_files) $(lib_object_files) $(libc_duck64)

build/boot/%.o: boot/%.asm
	@echo + nasm $@
	@mkdir -p $(shell dirname $@)
	@nasm -felf64 $< -o $@
