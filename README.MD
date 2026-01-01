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
- Special Number Handling (e.g. NaN and Infinity)

## Installation
```bash
git clone https://github.com/JensT1999/CSlidingWindowMedian.git
```

## Usage
Please note: Compiling the **sliding median window** expects using the GCC or clang compiler. Especially the MSVC compiler is not supported. <br> To compile properly you need to install GCC or clang from their official
website.
### Python
If you want to use the **sliding median window** in python you need to install setuptools and cython. <br>
To install both go to your python environment and type:
```bash
pip install setuptools cython
```

In Python the **sliding median window** works with NumPy arrays. This is the reason you have to install
NumPy too by running the command:<br>
```bash
pip install numpy
```

After installing everything that is needed skip to the python directory of this project. <br>
Note: Stay in your Python environment! (Also if you use a virtual python environment). <br>
Setup the Python binding by running the following command in the python directoy of this project:
```bash
python setup.py build_ext --inplace
```

Then import NumPy and the sliding median function:
```python
import numpy as np
from sliding_median_window import sliding_median_window
```

After that use the **sliding median window** with the following syntax:
```python
sliding_median_window(input_array (NumPy Array),
    windowSize,
    steps,
    ignoreNaNWindows,
    output_array (Numpy Array))
```

Explanation of input parameters:
- input_array: NumPy array containing your data.
- windowSize: Size of the sliding window.
- steps: Step size for the window (the function computes a result every steps elements).
- ignoreNaNWindows: true -> If at least one NaN is present in the window, the resulting median for that window will be NaN <br> false -> The median is calculated using only valid (non-NaN) elements inside the window. Infinite values (+/-INFINITY) are considered valid. If no valid elements remain in the window, the resulting median will be NaN. Example: valid_elements = windowSize - NaN_count
- output_array: NumPy array to store the results. Any existing data in this array will be overwritten.

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
# When ignoring NaN Windows
sliding_median_window(arr, windowSize, steps, True, output)

# When not ignoring NaN Windows (e.g. calculating the median of the valid elements remaining in the window)
sliding_median_window(arr, windowSize, steps, False, output)
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
    bool ignoreNaNWindows,
    double *outputArray);
```

Please note: The parameters of the C function are the same as the python function.

### Important
Please note that in both implementations the size of the result array should be at least:<br>
<b>((input_array_length - windowSize) / steps + 1)</b><br><br>
Please note that setting ignoreNaNWindows to false results in the median no longer being calculated over the full window size if a NaN is present.<br>

<b>Example:</b>
windowSize = 10, NaN count in the window = 1
→ The median will be calculated from the remaining 9 valid elements.

### Makefile for C Usage
Due to the project structure, no Makefile is included by default.
Below is an example Makefile that can be used to compile the project in C:
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

### Makefile for tests
The project also provides a Makefile to build the test executable. Simply navigate to the project’s root directory and run the following command:
```bash
make -f Makefile.test
```
The command will create an executable **run_tests** file. The corresponding tests can be found in the test directory.<br>
To clean all files created by the above command run:
```bash
make -f Makefile.test clean
```

## Contact
If you have any questions, feel free to contact me:
jens.trappmann@icloud.com
