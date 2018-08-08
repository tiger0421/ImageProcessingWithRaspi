import numpy as np
import cv2
import time


def LSMETHOD(x, y, middle_pnum):
    print(np.polyfit(y, x, 1))



t1 = time.time()
for a in range(1):
#    img = cv2.imread('line_to_left.png')
#    img = cv2.imread('noise_line.png')
#    img = cv2.imread('bracky_line2.png')
    img = cv2.imread('raspicamera.jpg')
    middle_point_xcoordinate = np.zeros(6, dtype = np.int16)
    middle_point_ycoordinate = np.zeros(6, dtype = np.uint16)
    middle_pnum = 0

    for line_num in range(6):
        upper = 40*line_num + 15
        lower = upper + 3

#        imgray = cv2.cvtColor(img[upper:lower, 328:2952], cv2.COLOR_BGR2GRAY)
        imgray = cv2.cvtColor(img[upper:lower, 820:2460], cv2.COLOR_BGR2GRAY)
# Otsu's thresholding after Gaussian filtering
        blur = cv2.GaussianBlur(imgray,(5,5),0)
        ret,threshold = cv2.threshold(blur,0,255,cv2.THRESH_BINARY+cv2.THRESH_OTSU)

        kernel = np.array([[0, 0, 0],
                    [-1, 0, 1],
                    [0, 0, 0]])
# differential
        dst = np.uint8(np.abs(cv2.filter2D(threshold, cv2.CV_64F, kernel)))

        min_contours = 3280
        max_contours = 0
        for i in range(1640):
            if dst[1][i] > 90:
                if i < min_contours:
                    min_contours = i
                if i > max_contours:
                    max_contours = i+2
        a_line_center = int(int(min_contours)+int(max_contours))/2
        print(a_line_center)
        if a_line_center > 100:
            middle_point_xcoordinate[line_num] = a_line_center - 820
            middle_point_ycoordinate[line_num] = upper + 1
            middle_pnum += 1

    if middle_pnum == 0:
        break
    if cv2.waitKey(1) & 0xff == ord('q'):
        break

LSMETHOD(middle_point_xcoordinate, middle_point_ycoordinate, middle_pnum)
t2 = time.time()

print("time = " + str(t2-t1) + "[sec]")
tempimg = img.copy()
gray = cv2.cvtColor(tempimg, cv2.COLOR_BGR2GRAY)
for line_num in range(6):
    upper = 40*line_num + 15
    a = middle_point_xcoordinate[line_num]
    tempimg[upper-15:upper+15, 820:2460] = [0, 0, 255]
#    tempimg[upper-15:upper+15, 820:2460] = [0, 0, 255]
    tempimg[upper-15:upper+15, int(a-15+1640):int(a+15+1640)] = [255, 255, 0]
cv2.imwrite("orbit.png", tempimg)
cv2.imwrite("gray.png",gray)
