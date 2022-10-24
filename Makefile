CXXFLAGS = -O0 -g -std=c++20

.PHONY: all clean plot-dxdt plot-dvdt plot-tentai

all: build/joho1 build/joho2 build/keta build/marume build/dxdt build/dvdt build/tentai

clean:
	rm -rf build output

build/%: src/%.cpp
	mkdir -p build
	clang++ ${CXXFLAGS} -o $@ $<

output/dxdt: build/dxdt scripts/run-dxdt.sh Makefile
	mkdir -p output
	rm output/dxdt
	scripts/run-dxdt.sh

plot-dxdt: output/dxdt Makefile
	gnuplot -persist scripts/plot-dxdt.plt

output/dvdt: build/dvdt Makefile
	echo -e "8\n800\n" | build/dvdt 2> output/dvdt

plot-dvdt: output/dvdt Makefile
	gnuplot -persist scripts/plot-dvdt.plt

output/tentai: build/tentai Makefile
	echo -e "4\n10000\n" | build/tentai 2> output/tentai

plot-tentai: output/tentai Makefile
	gnuplot -persist scripts/plot-tentai.plt
