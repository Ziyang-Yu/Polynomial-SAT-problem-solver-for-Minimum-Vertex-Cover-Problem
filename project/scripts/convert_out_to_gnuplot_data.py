#!/usr/bin/env python3

import sys
import statistics

timeInMicrosecond_cnf_sat = []
timeInMicrosecond_approx_1 = []
timeInMicrosecond_approx_2 = []
ratio_approx_1 = []
ratio_approx_2 = []

nVertex = int(sys.argv[1])
vcOutputFile = open(sys.argv[2], 'r')

while True:
    line = vcOutputFile.readline()
    if line == '':
        break
    line = line.strip()
    pos = line.find(':')+1
    rawValue = line[pos:]
    if line.startswith('CNF-SAT-VC(microsecond)'):
        timeInMicrosecond_cnf_sat.append(int(rawValue))
    elif line.startswith('APPROX-VC-1(microsecond)'):
        timeInMicrosecond_approx_1.append(int(rawValue))
    elif line.startswith('APPROX-VC-2(microsecond)'):
        timeInMicrosecond_approx_2.append(int(rawValue))
    elif line.startswith('APPROX-VC-1(ratio)'):
        ratio_approx_1.append(float(rawValue))
    elif line.startswith('APPROX-VC-2(ratio)'):
        ratio_approx_2.append(float(rawValue))

vcOutputFile.close()



gnuplot_time_cnf_sat = open('gnuplot_time_cnf_sat.dat', 'a')
gnuplot_time_cnf_sat.write('{0:d} {1:.2f} {2:.2f}\n'.format(nVertex, statistics.mean(timeInMicrosecond_cnf_sat), statistics.stdev(timeInMicrosecond_cnf_sat)))
gnuplot_time_cnf_sat.close()

gnuplot_time_approx_1 = open('gnuplot_time_approx_1.dat', 'a')
gnuplot_time_approx_1.write('{0:d} {1:.2f} {2:.2f}\n'.format(nVertex, statistics.mean(timeInMicrosecond_approx_1), statistics.stdev(timeInMicrosecond_approx_1)))
gnuplot_time_approx_1.close()

gnuplot_time_approx_2 = open('gnuplot_time_approx_2.dat', 'a')
gnuplot_time_approx_2.write('{0:d} {1:.2f} {2:.2f}\n'.format(nVertex, statistics.mean(timeInMicrosecond_approx_2), statistics.stdev(timeInMicrosecond_approx_2)))
gnuplot_time_approx_2.close()

gnuplot_ratio_approx_1 = open('gnuplot_ratio_approx_1.dat', 'a')
gnuplot_ratio_approx_1.write('{0:d} {1:.2f} {2:.2f}\n'.format(nVertex, statistics.mean(ratio_approx_1), statistics.stdev(ratio_approx_1)))
gnuplot_ratio_approx_1.close()

gnuplot_ratio_approx_2 = open('gnuplot_ratio_approx_2.dat', 'a')
gnuplot_ratio_approx_2.write('{0:d} {1:.2f} {2:.2f}\n'.format(nVertex, statistics.mean(ratio_approx_2), statistics.stdev(ratio_approx_2)))
gnuplot_ratio_approx_2.close()
