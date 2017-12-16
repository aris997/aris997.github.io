set logscale x
plot 'output.dat' u 2:4 w p lc rgb 'red'

set terminal pdf color enhanced size 20cm,20cm 
set output 'graph_enviromet.pdf'
set logscale x
set grid
set size square
set xlabel 't'
set ylabel 'x^2(t)'
set title 'Media su 100 medie di 10^4 realizzazioni'
plot 'output.dat' u 2:4 w p lc rgb 'red'