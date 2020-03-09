lib_source_files := $(wildcard lib/*.cpp)
lib_object_files := $(patsubst lib/%.cpp, \
		build/lib/%.o, $(lib_source_files))

LIB_CXX := $(CXX) -O2

build/lib/%.o: lib/%.cpp $(header_files)
	@echo + cxx $@
	@mkdir -p $(shell dirname $@)
	@$(LIB_CXX) -c $< -o $@
