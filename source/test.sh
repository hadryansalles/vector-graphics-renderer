#!/bin/bash
inputs=`ls ../rvgs/*.rvg`
driver='driver.png'
program='process.lua'
lua='luapp5.3'

for input in $inputs
do
    filename=$(basename -- "$input")
    extension="${filename##*.}"
    filename="${filename%.*}"
    [ ! -d "../png-out" ] && mkdir ../png-out
    output="../png-out/"$filename".png" 
    $lua $program $driver $input $output
done