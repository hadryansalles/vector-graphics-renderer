#!/bin/bash
inputs=`ls ../rvgs/*.rvg`
outputs="../pngs-out/"
driver='driver.hadryan_salles'
program='process.lua'
lua='luapp5.3'

[ ! -d $outputs ] && mkdir $outputs
rm $outputs*
START=$(date +%s.%N)
for input in $inputs
do
    filename=$(basename -- "$input")
    extension="${filename##*.}"
    filename="${filename%.*}"
    output=$outputs$filename".png" 
    $lua $program $driver $input $output $@
done
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "TOTAL TIME: $DIFF"
