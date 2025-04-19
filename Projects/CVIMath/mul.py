import numpy as np
import time

SIZE = 100

gl_elapsed = 0
for i in range(1000):

    # Generate random matrices
    A = np.random.randint(0, 10, (SIZE, SIZE))
    B = np.random.randint(0, 10, (SIZE, SIZE))

    # Measure time taken for multiplication
    start_time = time.time()
    C = np.dot(A, B)
    end_time = time.time()

    # Print time taken
    gl_elapsed += (end_time - start_time) * 1000
    # print(f"Time taken for matrix multiplication: {:.2f} ms")

    # Print a small portion of the result matrix
    print("Sample output from the result matrix:")
    print(C[:5, :5])


print(gl_elapsed / 1000 )