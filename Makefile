CXXFLAGS = -O0 -g -std=c++20

.PHONY: all

all: build/joho1 build/joho2 build/keta build/marume

build/%: src/%.cpp
	mkdir -p build
	clang++ ${CXXFLAGS} -o $@ $^
