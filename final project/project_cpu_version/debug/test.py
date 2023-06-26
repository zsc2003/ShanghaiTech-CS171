import numpy as np
import cv2

src = cv2.imread("result.png", 0)
src_rgb = cv2.cvtColor(src, cv2.COLOR_GRAY2RGB)
cv2.imwrite("rgb.png", src_rgb)