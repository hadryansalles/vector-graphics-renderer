#!/usr/bin/python3.6
import cv2
import numpy as np
import os

gtp = "../pngs-gt/"
outp = "../pngs-out/"
gt = os.listdir(gtp)
out = os.listdir(outp)

for i in range(len(gt)) :
    if gt[i] != out[i] : 
        print("error. missing file", gt[i]) 
        exit()
    o = cv2.imread(gtp+gt[i])
    d = cv2.imread(outp+out[i])
    if o.shape != d.shape : 
        print("error. images with different shapes", gt[i])
        exit()
    dif = cv2.subtract(o, d)
    b, g, r = cv2.split(dif)
    if cv2.countNonZero(b) != 0 or cv2.countNonZero(g) != 0 or cv2.countNonZero(r) != 0 :
        print("error. images aren't equal", gt[i], out[i])
        os.system("open " + gtp+gt[i])
        os.system("open " + outp+out[i])
        exit()

print("complete.")
    