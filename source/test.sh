#!/bin/bash
inputs=`ls ../rvgs/*.rvg`
outputs="../pngs-out/"
driver='driver.png'
program='process.lua'
lua='luapp5.3'
heavy=("blue_butterfly"
       "anatomical_heart"
       "paris-30k"
       "blender_freestyle"
       "bunny_dense_flat"
       "carrera"
       "red_racing_car"
       "relatorio"
       "colorado"
       "samurai"
       "welsh_dragon"
       "wormian_bones"
       "contour"
       "hawaii"
       "page_1"
       "page_2"
       "pirate-with-knife"
       "puss-in-boots"
       "roads"
       "inkboard"
       "spiral"
       "spirograph")

if [[ "$1" =~ "heavy" ]]; then
    heavy=()
fi

[ ! -d $outputs ] && mkdir $outputs
rm $outputs*
START=$(date +%s.%N)
for input in $inputs
do
    filename=$(basename -- "$input")
    extension="${filename##*.}"
    filename="${filename%.*}"
    output=$outputs$filename".png" 
    if [[ ! " ${heavy[@]} " =~ " ${filename} " ]]; then
        $lua $program $driver $input $output
    fi
done
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "TOTAL TIME: $DIFF"
