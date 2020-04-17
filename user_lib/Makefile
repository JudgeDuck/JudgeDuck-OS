user_lib_header_files := $(wildcard user_lib/*.hpp)
user_lib_source_files := $(wildcard user_lib/*.cpp)
user_lib_asm_source_files := $(wildcard user_lib/*.S)
user_lib_object_files := $(patsubst user_lib/%.cpp, \
		build/user_lib/%.o, $(user_lib_source_files))

user_lib_object_files += $(patsubst user_lib/%.S, \
		build/user_lib/%.o, $(user_lib_asm_source_files))

USER_LIB_CXX := $(CXX) -O2 -static -nostdlib -U_FORTIFY_SOURCE

build/user_lib/%.o: user_lib/%.cpp $(user_lib_header_files)
	@echo + cxx $@
	@mkdir -p $(shell dirname $@)
	@$(USER_LIB_CXX) -c $< -o $@

build/user_lib/%.o: user_lib/%.S $(user_lib_header_files)
	@echo + as $@
	@mkdir -p $(shell dirname $@)
	@$(USER_LIB_CXX) -c $< -o $@

build/user_lib/libduck.a: $(user_lib_object_files)
	@echo + ar $@
	@mkdir -p $(shell dirname $@)
	@ar r $@ $(user_lib_object_files)
