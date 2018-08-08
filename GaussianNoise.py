import numpy as np
import cv2

img = cv2.imread("line_to_right.png")

noise = 50*np.random.randn(2464, 3280, 3)
img_n = img + noise

cv2.imwrite("noise_line_to_right.png", img_n)
