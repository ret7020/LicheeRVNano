import struct

import numpy as np
from scipy.spatial.distance import cosine



def cosine_distance(A, B):
    A_flat = A.flatten()
    B_flat = B.flatten()

    cos_sim = 1 - cosine(A_flat, B_flat)  
    coincidence_percentage = ((1 + cos_sim) / 2) * 100  

    return coincidence_percentage

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

bf16_value = int(input())
float_value = bf16_to_float(bf16_value)
print(f"bfloat16: {bf16_value} -> float: {float_value}")


truth = np.array([ 0.0844, -0.0290, -0.3003, -0.5378])
rel   = np.array( [0.10400390625, 0.0040283203125, -0.09521484375, -0.2431640625])

print(cosine_distance(truth, rel))
