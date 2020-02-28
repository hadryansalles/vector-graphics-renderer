!#usr/bin/bash
xi=-100
xf=100
yi=0
yf=0
dx=1
dy=1
in=$1
mkdir ../video-data
i=0
for ((x=$xi; x<=$xf; x+=$dx)) do
	luapp5.3 process.lua driver.png ../rvgs/$in.rvg ../video-data/$in-`printf %05d $i`.png -tx:$x $@ 
	((i++))
done
for ((y=$yi; y<=$yf; y+=$dy)) do
	luapp5.3 process.lua driver.png ../rvgs/$in.rvg ../video-data/$in-`printf %05d $i`.png -ty:$y $@ 
	((i++))
done
ffmpeg -framerate 30 -i ../video-data/$in-%05d.png -y ../videos-out/$in.mp4
open ../videos-out/$in.mp4
rm -rf ../video-data
