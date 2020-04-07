ducknet_header_files := $(wildcard ducknet/inc/*.h) $(wildcard ducknet/lib/*.h)
ducknet_source_files := $(wildcard ducknet/lib/*.cpp)
ducknet_object_files := $(patsubst ducknet/lib/%.cpp, \
		build/ducknet/%.o, $(ducknet_source_files))

DUCKNET_CXX := $(CXX) -O2

build/ducknet/%.o: ducknet/lib/%.cpp $(ducknet_header_files)
	@echo + cxx $@
	@mkdir -p $(shell dirname $@)
	@$(DUCKNET_CXX) -c $< -o $@

build/ducknet/libducknet.a: $(ducknet_object_files)
	@echo + ar $@
	@mkdir -p $(shell dirname $@)
	@ar r $@ $(ducknet_object_files)

libducknet := build/ducknet/libducknet.a
