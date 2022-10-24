set terminal x11
set xlabel "n"
set ylabel "1/e - x(1)"
set title "n and 1/e - x(1)"
set style data lines
plot "output/dxdt" title "1/e-x(1)"
