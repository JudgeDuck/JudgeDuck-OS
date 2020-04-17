user_lib32_header_files := $(wildcard user_lib32/*.hpp)
user_lib32_source_files := $(wildcard user_lib32/*.cpp)
user_lib32_asm_source_files := $(wildcard user_lib32/*.S)
user_lib32_object_files := $(patsubst user_lib32/%.cpp, \
		build/user_lib32/%.o, $(user_lib32_source_files))

user_lib32_object_files += $(patsubst user_lib32/%.S, \
		build/user_lib32/%.o, $(user_lib32_asm_source_files))

USER_LIB32_CXX := $(CXX) -m32 -O2 -static -nostdlib -U_FORTIFY_SOURCE

build/user_lib32/%.o: user_lib32/%.cpp $(user_lib32_header_files)
	@echo + cxx $@
	@mkdir -p $(shell dirname $@)
	@$(USER_LIB32_CXX) -c $< -o $@

build/user_lib32/%.o: user_lib32/%.S $(user_lib32_header_files)
	@echo + as $@
	@mkdir -p $(shell dirname $@)
	@$(USER_LIB32_CXX) -c $< -o $@

build/user_lib32/libduck.a: $(user_lib32_object_files)
	@echo + ar $@
	@mkdir -p $(shell dirname $@)
	@ar r $@ $(user_lib32_object_files)
