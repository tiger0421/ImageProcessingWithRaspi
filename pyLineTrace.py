# coding: UTF-8
import numpy as np
import cv2
import time


def LSMETHOD(x, y, middle_pnum):
    print(np.polyfit(y, x, 1))

#==== variable ====#
LOOPCNT = 1
LOOPTIME = 3
MAXHEIGHT = 200
RANGE = MAXHEIGHT / LOOPTIME
maskLower = np.array([0, 0, 0])
maskUpper = np.array([180, 255, 170])

cap = cv2.VideoCapture(0)
Img = cv2.imread('raspicamera.jpg')
#ret, Img = cap.read()
height, width, ch = Img.shape

for a in range(LOOPCNT):
#    ret, Img = cap.read()
    cpImg = Img.copy()

    start = time.time()

    for y in LOOPTIME:
        middle_point_xcoordinate = np.zeros(6, dtype = np.int16)
        middle_point_ycoordinate = np.zeros(6, dtype = np.uint16)
        upper = height - RANGE*y -3

        hsvImg = cv2.cvtColor(cpImg[upper:upper +3, 0:width], cv2.COLOR_BGR2HSV)
        maskRoi = cv2.inRange(hsvImg, maskLower, maskUpper)
        filteredImg = cv2.medianBlur(maskRoi, ksize=3)
        for i in width:
            if(filteredImg[1, i] == 255):
                aveXCoordinate += i
                count += 1
        aveXCoordinate /= (count+1)
        aveXCoordinate -= width / 2
        middle_point_xcoordinate[y] = aveXCoordinate
        middle_point_ycoordinate[y] = RANGE * y

        aveXCoordinate = 0
        count = 0
    if cv2.waitKey(1) & 0xff == ord('q'):
        break

    LSMETHOD(middle_point_xcoordinate, middle_point_ycoordinate, middle_pnum)
    end = time.time()
    aveTime += (end - start)

print("time = " + str(aveTime/y) + "[sec]" + str(y))
