kernel := build/kernel.bin
kernel_stripped := build/kernel_stripped.bin
iso := build/os-legacy.iso
esp := build/esp
grub_efi := build/grub.efi
esp_files := build/esp_files_done

CXX := g++
CXX += -std=c++14
CXX += -Wall -Wextra -Werror -march=x86-64
CXX += -fno-strict-aliasing  # prevent ub!
CXX += -mno-red-zone
CXX += -U_FORTIFY_SOURCE
CXX += -I .
CXX += -I ducknet/inc/

linker_script := boot/linker.ld
grub_cfg := boot/grub.cfg
assembly_source_files := $(wildcard boot/*.asm)
assembly_object_files := $(patsubst boot/%.asm, \
		build/boot/%.o, $(assembly_source_files))

kern_asm_source_files := $(wildcard kern/*.S)
kern_asm_object_files := $(patsubst kern/%.S, \
		build/kern/%.o, $(kern_asm_source_files))

header_files := $(wildcard inc/*.h) $(wildcard inc/*.hpp) $(wildcard ducknet/inc/*.h)
LIB_PREFIX := ../duck-binaries/duck64/lib/
LIBC_PREFIX := $(LIB_PREFIX)musl/
libstdcxx_files := $(LIB_PREFIX)libstdc++.a
libc_files := -L $(LIBC_PREFIX) -L $(LIB_PREFIX) -lc -lgcc -lgcc_eh -lc
libc_crt_start := $(LIBC_PREFIX)crt1.o $(LIBC_PREFIX)crti.o
libc_crt_end := $(LIBC_PREFIX)crtn.o

i386_LIB_PREFIX := ../duck-binaries/duck32/lib/
i386_LIBC_PREFIX := $(i386_LIB_PREFIX)musl/
i386_libstdcxx_files := $(i386_LIB_PREFIX)libstdc++.a
i386_libc_files := -L $(i386_LIBC_PREFIX) -L $(i386_LIB_PREFIX) -lc -lgcc -lgcc_eh -lc
i386_libc_crt_start := $(i386_LIBC_PREFIX)crt1.o $(i386_LIBC_PREFIX)crti.o
i386_libc_crt_end := $(i386_LIBC_PREFIX)crtn.o

all: $(kernel) $(kernel_stripped)

include kern/Makefile
include lib/Makefile
include user_lib/Makefile
include user/Makefile
include user32/Makefile
include user_lib32/Makefile
include ducknet/lib/Makefile

.PHONY: all clean

clean:
	@rm -r build/*

QEMUOPTS ?= -m 2048M

run-legacy: $(iso)
	@qemu-system-x86_64 -serial mon:stdio -cdrom $(iso) -cpu Skylake-Client $(QEMUOPTS)

run-legacy-nox: $(iso)
	@qemu-system-x86_64 -serial mon:stdio -nographic -cdrom $(iso) -cpu Skylake-Client $(QEMUOPTS)

QEMU_UEFI_OPTS := -bios /usr/share/OVMF/OVMF_CODE.fd
QEMU_UEFI_OPTS += -drive format=raw,file=fat:rw:$(esp)

run-uefi: $(esp_files)
	@qemu-system-x86_64 -serial mon:stdio $(QEMU_UEFI_OPTS) -cpu Skylake-Client $(QEMUOPTS)

run-uefi-nox: $(esp_files)
	@qemu-system-x86_64 -serial mon:stdio -nographic $(QEMU_UEFI_OPTS) -cpu Skylake-Client $(QEMUOPTS)

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub-mkrescue -o $(iso) build/isofiles -d /usr/lib/grub/i386-pc 2> /dev/null
	@rm -r build/isofiles

$(grub_efi): Makefile
	@echo 'regexp -s root [(]([^/)]*) $$cmdpath' > /tmp/grub.cfg
	@echo 'configfile /boot/grub/grub.cfg' >> /tmp/grub.cfg
	@grub-mkstandalone -d /usr/lib/grub/x86_64-efi --modules "multiboot2 part_gpt part_msdos" -O x86_64-efi "boot/grub/grub.cfg=/tmp/grub.cfg" -o $(grub_efi)

$(esp_files): $(kernel) $(grub_cfg) $(grub_efi)
	@mkdir -p $(esp)/boot/grub
	@mkdir -p $(esp)/efi/boot
	@cp $(kernel) $(esp)/boot/kernel.bin
	@cp $(grub_cfg) $(esp)/boot/grub
	@cp $(grub_efi) $(esp)/efi/boot/bootx64.efi
	@touch $(esp_files)

$(kernel): $(assembly_object_files) $(kern_object_files) $(lib_object_files) $(linker_script) \
	$(libc_duck64) $(kern_asm_object_files) $(libducknet) $(user_obj_files) $(user32_obj_files) $(libducknet)
	@echo + ld $(kernel)
	@ld -n -T $(linker_script) -o $(kernel) \
		$(libc_crt_start) $(assembly_object_files) $(kern_asm_object_files) $(kern_object_files) \
		$(lib_object_files) $(libducknet) $(libstdcxx_files) $(libc_files) $(libc_crt_end) \
		$(user_obj_files) $(user32_obj_files)

$(kernel_stripped): $(kernel)
	@echo + strip $(kernel_stripped)
	@cp $(kernel) $(kernel_stripped)
	@strip --strip-all $(kernel_stripped)

build/boot/%.o: boot/%.asm
	@echo + nasm $@
	@mkdir -p $(shell dirname $@)
	@nasm -felf64 $< -o $@

build/kern/%.o: kern/%.S
	@echo + as $@
	@mkdir -p $(shell dirname $@)
	@$(CXX) -c $< -o $@
