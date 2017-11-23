set term postscript enhanced color

set output 'vt_eulerocromer.ps'
set xlabel 't'
set ylabel 'v(t)'
set yrange [-5:5]
set title 'Grafico Velocita - Tempo, Eulero-Cromer'
plot 'output.dat' u 1:3 w l 