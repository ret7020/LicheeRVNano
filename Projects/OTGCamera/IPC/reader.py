import cv2
import numpy as np
import posix_ipc
import mmap
import time

FRAME_WIDTH, FRAME_HEIGHT, CHANNELS = 320, 240, 3
FRAME_SIZE = FRAME_WIDTH * FRAME_HEIGHT * CHANNELS
SHM_NAME = "/frame_shm"

shm = posix_ipc.SharedMemory(SHM_NAME, posix_ipc.O_CREAT, size=FRAME_SIZE)
mapfile = mmap.mmap(shm.fd, FRAME_SIZE)
shm.close_fd()

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, FRAME_WIDTH)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT)

try:
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        frame_bytes = frame.tobytes()

        mapfile.seek(0)
        mapfile.write(frame_bytes)

except KeyboardInterrupt:
    print("Interrupted by user")

finally:
    cap.release()
    mapfile.close()
    posix_ipc.unlink_shared_memory(SHM_NAME)

