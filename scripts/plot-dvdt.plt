set terminal x11
set xlabel "t" 
set ylabel "x(t)" 
set title "t and x(t)"
set style data lines
plot "output/dvdt",cos(x) title "cos(t)"
