set term postscript enhanced color
set output 'plot1.ps'
set xlabel 't'
set ylabel 'x(t)'
set yrange [-5:5]
set title 'Spazio-Tempo, Eulero-Cromer'
plot 'output.dat' u 1:2 w l

set output 'plot2.ps'
set xlabel 't'
set ylabel 'v(t)'
set yrange [-5:5]
set title 'Grafico Velocita' - Tempo, Eulero-Cromer'
plot 'output.dat' u 1:3 w l 

set output 'e(t)_e(0)_Eulero_Cromer.ps'
set xlabel 'x(t)'
set ylabel 'v(t)'
plot 'output.dat' u 2:3 w l 