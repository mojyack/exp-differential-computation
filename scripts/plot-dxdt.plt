set terminal x11
set xlabel "n" 
set ylabel "x(1)" 
set title "n and x(1)"
set style data lines
plot "output/dxdt", exp(-1)
