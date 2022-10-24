CXXFLAGS = -O0 -g -std=c++20

.PHONY: all clean plot-dxdt

all: build/joho1 build/joho2 build/keta build/marume build/dxdt

clean:
	rm -rf build output

build/%: src/%.cpp
	mkdir -p build
	clang++ ${CXXFLAGS} -o $@ $^

output/dxdt: build/dxdt scripts/run-dxdt.sh
	mkdir -p output
	rm output/dxdt
	scripts/run-dxdt.sh

plot-dxdt: output/dxdt
	gnuplot -persist scripts/plot-dxdt.plt
