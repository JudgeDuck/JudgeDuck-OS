user_header_files := $(wildcard user/*.hpp)
user_source_files := $(wildcard user/*.cpp)
user_elf_files := $(patsubst user/%.cpp, \
		build/user/%.elf, $(user_source_files))
user_obj_files := $(patsubst user/%.cpp, \
		build/user/%.obj, $(user_source_files))

USER_CXX := g++ -O2 -std=c++14 -static -nostdlib -U_FORTIFY_SOURCE -fPIC -Wl,-z,max-page-size=4096

build/user/%.elf: user/%.cpp $(user_header_files) build/user_lib/libduck.a
	@echo + cxx[USER] $@
	@mkdir -p $(shell dirname $@)
	@$(USER_CXX) -o $@ \
		$(libc_crt_start) $< \
		$(libstdcxx_files) $(libc_files) build/user_lib/libduck.a $(libc_files) $(libc_crt_end)

build/user/%.obj: build/user/%.elf
	@echo + ld-binary $@
	@mkdir -p $(shell dirname $@)
	@cd $(shell dirname $@) && \
		ld -r -b binary -o $(patsubst build/user/%,%,$@) $(patsubst build/user/%,%,$<)
