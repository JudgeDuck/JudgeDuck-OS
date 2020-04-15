user32_header_files := $(wildcard user32/*.hpp)
user32_source_files := $(wildcard user32/*.cpp)
user32_elf_files := $(patsubst user32/%.cpp, \
		build/user32/%.elf, $(user32_source_files))
user32_obj_files := $(patsubst user32/%.cpp, \
		build/user32/%.obj, $(user32_source_files))

USER32_CXX := g++ -m32 -O2 -std=c++14 -static -nostdlib -U_FORTIFY_SOURCE -fPIC

build/user32/%.elf: user32/%.cpp $(user32_header_files)
	@echo + cxx[USER] $@
	@mkdir -p $(shell dirname $@)
	@$(USER32_CXX) -o $@ \
		$<

build/user32/%.obj: build/user32/%.elf
	@echo + ld-binary $@
	@mkdir -p $(shell dirname $@)
	@cd $(shell dirname $@) && \
		ld -r -b binary -o $(patsubst build/user32/%,%,$@) $(patsubst build/user32/%,%,$<)
