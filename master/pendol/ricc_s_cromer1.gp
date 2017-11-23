set term postscript eps enhanced color
set output '|ps2pdf - p_cromer1A.pdf'
set title 'Pendolo Semplice - Cromer - grafico angolo'
set xlabel 't'
set ylabel 'alpha'
plot 'p_cromer1.dat' u 1:2 w l

set term postscript eps enhanced color
set output '|ps2pdf - p_cromer1V.pdf'
set title 'Pendolo Semplice - Cromer - grafico velocità angolare'
set xlabel 't'
set ylabel 'v Ang'
plot 'p_cromer1.dat' u 1:3 w l

set term postscript eps enhanced color
set output '|ps2pdf - p_cromer1E.pdf'
set title 'Pendolo Semplice - Cromer - grafico Energia'
set xlabel 't'
set ylabel 'E(t)/E0 - 1'
plot 'p_cromer1.dat' u 1:4 w l