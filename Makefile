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

kern_asm_source_files := $(wildcard kern/*.S)
kern_asm_object_files := $(patsubst kern/%.S, \
		build/kern/%.o, $(kern_asm_source_files))

header_files := $(wildcard inc/*.h) $(wildcard inc/*.hpp)
LIB_PREFIX := ../duck-binaries/duck64/lib/
LIBC_PREFIX := $(LIB_PREFIX)musl/
libstdcxx_files := $(LIB_PREFIX)libstdc++.a
libc_files := -L $(LIBC_PREFIX) -L $(LIB_PREFIX) -lc -lgcc -lgcc_eh -lc
libc_crt_start := $(LIBC_PREFIX)crt1.o $(LIBC_PREFIX)crti.o
libc_crt_end := $(LIBC_PREFIX)crtn.o

all: $(kernel)

include kern/Makefile
include lib/Makefile
include user_lib/Makefile
include user/Makefile

.PHONY: all clean run iso

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

$(kernel): $(assembly_object_files) $(kern_object_files) $(lib_object_files) $(linker_script) $(libc_duck64) $(kern_asm_object_files) $(user_obj_files)
	@echo + ld $(kernel)
	@ld -n -T $(linker_script) -o $(kernel) \
		$(libc_crt_start) $(assembly_object_files) $(kern_asm_object_files) $(kern_object_files) \
		$(lib_object_files) $(libstdcxx_files) $(libc_files) $(libc_crt_end) $(user_obj_files)

build/boot/%.o: boot/%.asm
	@echo + nasm $@
	@mkdir -p $(shell dirname $@)
	@nasm -felf64 $< -o $@

build/kern/%.o: kern/%.S
	@echo + as $@
	@mkdir -p $(shell dirname $@)
	@$(CXX) -c $< -o $@
