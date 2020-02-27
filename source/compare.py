#!/usr/bin/python3.6
import cv2
import os

gtp = "../pngs-gt/"
outp = "../pngs-out/"
gt = os.listdir(gtp)
out = os.listdir(outp)

for img in gt :
    if not out.__contains__(img) :
        print("error. missing file", img) 
    else :
        o = cv2.imread(gtp+img)
        d = cv2.imread(outp+img)
        if o.shape != d.shape : 
            print("error. images with different shapes", img)
        else :    
            dif = cv2.subtract(o, d)
            b, g, r = cv2.split(dif)
            if cv2.countNonZero(b) != 0 or cv2.countNonZero(g) != 0 or cv2.countNonZero(r) != 0 :
                abs_dif = (sum(sum(r)) + sum(sum(g)) + sum(sum(b))) / (255*o.shape[0]*o.shape[1]*o.shape[2])
                print("error. images aren't equal", img, "difference absolute of", "%.3f" % abs_dif)
print("complete.")
    