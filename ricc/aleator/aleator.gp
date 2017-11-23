set term x11 0 persist enhanced
set grid
plot "media.dat" u 1:2 w l lc rgb "red"

set term x11 1 persist enhanced
set log
plot "media.dat" u 1:3 w p lc rgb "red"
unset log

reset
set term x11 2 persist enhanced
set grid ytics linestyle 1 lc rgb "black"
set style fill solid 1.00 border 7
set style histogram
plot "posi.dat" w boxes lc rgb "red"
unset style
