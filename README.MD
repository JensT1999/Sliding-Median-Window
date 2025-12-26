# Sliding Median Window
## Overview

This project implements a **sliding median window** with Python/Cython bindings.
It supports different strategies based on the size of the window:
- **Median Networks** for tiny windows (size 2-8)
- **Double-Heap Approach** for bigger windows

This library allows efficient computation of the median over a sliding window in numerical sequences.

## Features
- Sliding median computation for arbitrary window sizes
- Optimized for performance using median networks and double-heaps in C
- Python interface via Cython for easy integration with NumPy

## Installation
```bash
git clone https://github.com/JensT1999/CSlidingWindowMedian.git
```

## Usage
### Python
First, setup the Python binding by running the following command in the python directoy:
```bash
python setup.py build_ext --inplace
```

Then import NumPy and the sliding median function:
```python
import numpy as np
from sliding_median_window import sliding_median_window
```

After that use the sliding window with the following syntax:
```python
sliding_median_window(input_array (NumPy Array),
    windowSize,
    steps,
    output_array (Numpy Array))
```

Example usage:
```python
# Setting up the window
n = 100_000
windowSize = 100
steps = 1

# Creating random input array
arr = np.random.uniform(low=1.0, high=1000.0, size=n)

# Creating the output array
output = np.empty(n)

# Calling the sliding window
sliding_median_window(arr, windowSize, steps, output)
```
Note: The Python function is a wrapper around the C implementation, working directly with NumPy arrays.

### C
To use the sliding median window in C, include the API header:
```c
#include "medianwindow_api.h"
```

Then call the following function:
```c
bool sliding_medianwindow(double *inputArray,
    size_t length,
    size_t windowSize,
    size_t steps,
    double *outputArray);
```

### Important
Please note that in both implementations the size of the result array should be at least:<br>
<b>((input_array_length - windowSize) / steps + 1)

### Makefile for C Usage
Due to the project structure, using a Makefile is recommended. An example Makefile is shown below:
```makefile
CC	 = gcc
CFLAGS	 = -O3 -march=native -flto -Wall -Wextra -std=c99 -Iinclude -Isrc

TARGET	 = outRelease

SRC	 = $(wildcard src/*.c)
OBJ	 = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
```

## Contact
If you have any questions, feel free to contact me:
jens.trappmann@icloud.com
