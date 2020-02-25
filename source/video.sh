xi=0
xf=50
dx=2
in=triangle1
i=0
for ((x=$xi; x<=$xf; x+=dx)) do
	((i++))
	luapp5.3 process.lua driver.png ../rvgs/$in.rvg ../video/$in-$i.png -tx:$x
done

ffmpeg -framerate 5 -i ../video/$in-%d.png -y ../video/$in.mp4
rm ../video/*.png
open ../video/$in.mp4
