#! /usr/bin/bash

# generate test inputs with test_*.in
# generate a new script run.sh taking test_*.in and output to test_*.out

runCommandFileName=run.sh
convertOutputFileName=convert_out_to_gnuplot_data.py
vcProgramFilePath=../build/ece650-prj
min=5
max=50
inc=5
nGraphPerV=10
nDuplicateRun=10

rm *.dat
rm *.in
rm *.out

> $runCommandFileName
chmod a+x $runCommandFileName

for ((v=$min; v <= $max; v += $inc))
do
    vcInputFileName=test_$v.in
    vcOutputFileName=test_$v.out

    echo "$vcProgramFilePath < $vcInputFileName > $vcOutputFileName" >> $runCommandFileName
    echo "./$convertOutputFileName $v $vcOutputFileName" >> $runCommandFileName

    for ((i=0; i < $nGraphPerV; i++))
    do
        input=$(./graphGen $v)
        for ((j=0; j < $nDuplicateRun; j++))
        do
            echo "$input" >> $vcInputFileName
        done
    done
done


echo "gnuplot -p running_time_1.gp" >> $runCommandFileName
echo "gnuplot -p running_time_2.gp" >> $runCommandFileName
echo "gnuplot -p approximation_ratio.gp" >> $runCommandFileName
