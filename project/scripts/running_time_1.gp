#!/usr/local/bin/gnuplot -persist

set terminal pngcairo transparent enhanced font "arial,10" fontscale 1.0 size 600, 400
set output 'running_time_1.png'

set autoscale
set offsets graph 0.01, 0.01, 0.2, 0
set xtics 5
set key left
set style data lines
set xlabel "Number of vertices"
set ylabel "Running time ({/Symbol m}s)"

NO_ANIMATION = 1
plot [] [0:1000] \
    "gnuplot_time_cnf_sat.dat" using 1:2:3 with yerrorbars title "CNF-SAT" lt rgb "#000000", \
    "gnuplot_time_cnf_sat.dat" using 1:2 with lines title "" lt rgb "#000000", \
    "gnuplot_time_approx_1.dat" using 1:2:3 with yerrorbars title "Approx-1" lt rgb "#000000", \
    "gnuplot_time_approx_1.dat" using 1:2 with lines title "" lt rgb "#000000", \
    "gnuplot_time_approx_2.dat" using 1:2:3 with yerrorbars title "Approx-2" lt rgb "#000000", \
    "gnuplot_time_approx_2.dat" using 1:2 with lines title "" lt rgb "#000000"