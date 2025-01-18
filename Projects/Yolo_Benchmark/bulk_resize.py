import cv2, os, sys

imgs = os.listdir(sys.argv[1])
for i in imgs:
    img = cv2.imread(os.path.join(sys.argv[1], i))
    img = cv2.resize(img, (int(sys.argv[2]), int(sys.argv[3])))
    cv2.imwrite(os.path.join(sys.argv[1], i), img)
