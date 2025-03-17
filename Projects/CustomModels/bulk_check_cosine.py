import os
import numpy as np
from scipy.spatial.distance import cosine

files = os.listdir("./out")

def bf16_to_float(bf16):
    binary_str = format(bf16, '016b')

    sign = int(binary_str[0], 2)
    exponent = int(binary_str[1:9], 2)
    mantissa = int(binary_str[9:], 2)

    if exponent == 0:
        value = (-1) ** sign * (mantissa / (2 ** 7)) * (2 ** (-126))
    else:
        value = (-1) ** sign * (1 + mantissa / (2 ** 7)) * (2 ** (exponent - 127))

    return value


def cosine_distance(A, B):

    cos_sim = 1 - cosine(A, B)
    coincidence_percentage = ((1 + cos_sim) / 2) * 100

    return coincidence_percentage


sm = []

for file in files:
    board = np.load(f"./out/{file}")
    outs_board = board["fc3_1_Gemm"].flatten()
    outs_board = list(map(lambda x: bf16_to_float(x), outs_board))
    #print(outs_board)

    orig = np.load(f"/tmp/grnd/content/test_measure/out/{file}")
    outs_orig = orig["array_2d"].flatten()
    sm.append(cosine_distance(outs_board, outs_orig))

print(sum(sm) / len(sm))



