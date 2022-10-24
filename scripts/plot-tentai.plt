set terminal x11
set xlabel "x" 
set ylabel "y" 
set title "two planets"
set style data lines
plot "output/tentai" using 1:2 title "p1", "output/tentai" using 3:4 title "p2"
