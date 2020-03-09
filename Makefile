kernel := build/kernel.bin
iso := build/os.iso

CXX := g++ -march=x86-64 -mno-mmx -mno-sse -mno-red-zone

linker_script := boot/linker.ld
grub_cfg := boot/grub.cfg
assembly_source_files := $(wildcard boot/*.asm)
assembly_object_files := $(patsubst boot/%.asm, \
		build/boot/%.o, $(assembly_source_files))

include kern/Makefile

.PHONY: all clean run iso

all: $(kernel)

clean:
	@rm -r build/*

run: $(iso)
	@qemu-system-x86_64 -cdrom $(iso)

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub-mkrescue -o $(iso) build/isofiles -d /usr/lib/grub/i386-pc 2> /dev/null
	@rm -r build/isofiles

$(kernel): $(assembly_object_files) $(kern_object_files) $(linker_script)
	@ld -n -T $(linker_script) -o $(kernel) $(assembly_object_files) $(kern_object_files)

build/boot/%.o: boot/%.asm
	@echo + nasm $@
	@mkdir -p $(shell dirname $@)
	@nasm -felf64 $< -o $@
