kern_source_files := $(wildcard kern/*.cpp)
kern_object_files := $(patsubst kern/%.cpp, \
		build/kern/%.o, $(kern_source_files))

KERN_CXX := $(CXX) -O2

build/kern/%.o: kern/%.cpp
	@echo + cxx $@
	@mkdir -p $(shell dirname $@)
	@$(KERN_CXX) -c $< -o $@
