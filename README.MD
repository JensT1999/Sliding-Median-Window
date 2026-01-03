# Sliding Median Window
## Overview

This project implements a **sliding median window** with Python/Cython bindings.
It supports different strategies based on the size of the window:
- **Median/Sorting Networks** for tiny windows (size 2-8)
- **Double-Heap Approach** for bigger windows

This library allows efficient computation of the median over a sliding window in numerical sequences.

## Features
- Sliding median computation for arbitrary window sizes
- Optimized for performance using median/sorting networks and double-heaps in C
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
***
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
sliding_median_window(input_array,
    windowSize,
    steps,
    ignoreNaNWindows,
    output_array)
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

# When not ignoring NaN Windows
# (e.g. calculating the median of the valid elements remaining in the window)
sliding_median_window(arr, windowSize, steps, False, output)
```
Note: The Python function is a wrapper around the C implementation, working directly with NumPy arrays.

### C
***
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
<b>((input_array_length - windowSize) / steps + 1)</b><br>
Please note that setting ignoreNaNWindows to false results in the median no longer being calculated over the full window size if a NaN is present.<br>

<b>Example:</b>
windowSize = 10, NaN count in the window = 1
→ The median will be calculated from the remaining 9 valid elements.

## Makefiles
### Makefile for C Usage
***
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
***
The project also provides a Makefile to build the test executable. Simply navigate to the project's root directory and run the following command:
```bash
make -f Makefile.test
```
The command will create an executable **run_tests** file. The corresponding tests can be found in the test directory.<br>
To clean all files created by the above command run:
```bash
make -f Makefile.test clean
```

### Makefile for benchmarks
***
This project also provides a Makefile to build the benchmark executable. Simply navigate to the projects's root
directory and run the following command:
```bash
make -f Makefile.benchmark
```
The command will create an executable **run_benchmark** file. To run a benchmark first navigate to the benchmark
directory of this project, where the **run_benchmark** file is located. After that execute the following command:
```bash
./run_benchmark <inputSequenceLength>
                <nanValueCount>
                <infValueCount>
                <lowestPossibleValue>
                <heighestPossibleValue>
                <windowSize>
                <steps>
                <ignoreNaNWindows>
```
To clean all files created by the above command run:
```bash
make -f Makefile.benchmark clean
```

## Benchmarks
Below are the benchmarks, captured on a MacBook Air M3 using the **run_benchmark** executable mentioned above. <br>
For each measurement, 3 warm-up runs were performed, followed by 10 consecutive measurements. The mean and standard deviation were then calculated from these results. Both are represented in seconds and can be found in the following tables.

### Tiny Window Benchmarks (Median/Sorting Networks)
***
The following table presents benchmark results for sliding windows over input sequences containing no NaN or Infinity values. The ignoreNaNWindows property is set to false.
<table>
<tr><th>Input sequence length</th><th>Window size</th><th>Steps</th><th>Time taken (Mean)</th><th>Standard deviation</th></tr>
<tr><td>1000000</td><td>5</td><td>1</td><td>0.00953</td><td>0.00124</td></tr>
<tr><td>1000000</td><td>5</td><td>5</td><td>0.004536</td><td>0.00022</td></tr>
<tr><td>10000000</td><td>5</td><td>1</td><td>0.0446</td><td>0.00032</td></tr>
<tr><td>10000000</td><td>5</td><td>5</td><td>0.019665</td><td>0.00022</td></tr>
<tr><td>100000000</td><td>5</td><td>1</td><td>0.4651</td><td>0.0261</td></tr>
<tr><td>100000000</td><td>5</td><td>5</td><td>0.19861</td><td>0.00381</td></tr>
</table>
The following table presents measurements for sliding windows over an input sequence containing NaN and Infinity values. These values each account for 10% of the total input sequence length. The ignoreNaNWindows property is set to false.

<table>
<tr><th>Input sequence length</th><th>Window size</th><th>Steps</th><th>Time taken (Mean)</th><th>Standard deviation</th></tr>
<tr><td>1000000</td><td>5</td><td>1</td><td>0.0122</td><td>0.00049</td></tr>
<tr><td>1000000</td><td>5</td><td>5</td><td>0.00642</td><td>0.00034</td></tr>
<tr><td>10000000</td><td>5</td><td>1</td><td>0.06143</td><td>0.00094</td></tr>
<tr><td>10000000</td><td>5</td><td>5</td><td>0.02987</td><td>0.00033</td></tr>
<tr><td>100000000</td><td>5</td><td>1</td><td>0.62736</td><td>0.01572</td></tr>
<tr><td>100000000</td><td>5</td><td>5</td><td>0.30257</td><td>0.00577</td></tr>
</table>
The following table presents measurements for sliding windows over an input sequence containing NaN and Infinity values. These values each account for 10% of the total input sequence length. The ignoreNaNWindows property is set to true, which means that windows containing at least one NaN will result in NaN.

<table>
<tr><th>Input sequence length</th><th>Window size</th><th>Steps</th><th>Time taken (Mean)</th><th>Standard deviation</th></tr>
<tr><td>1000000</td><td>5</td><td>1</td><td>0.01173</td><td>0.00034</td></tr>
<tr><td>1000000</td><td>5</td><td>5</td><td>0.00595</td><td>0.00034</td></tr>
<tr><td>10000000</td><td>5</td><td>1</td><td>0.05884</td><td>0.00186</td></tr>
<tr><td>10000000</td><td>5</td><td>5</td><td>0.0286</td><td>0.0005</td></tr>
<tr><td>100000000</td><td>5</td><td>1</td><td>0.59473</td><td>0.02017</td></tr>
<tr><td>100000000</td><td>5</td><td>5</td><td>0.28486</td><td>0.0034</td></tr>
</table>

#### Interpretation for tiny windows
***
- Runtime scales linearly with the input sequence length.
- Increasing the step size significantly reduces runtime.
- Handling NaN and Infinity values introduces a measurable but predictable overhead.
***

### Big Window Benchmarks (Double-Heap Approach)
***
The following table presents benchmarks for larger window sizes, where the double-heap approach is used. The input sequence contains no NaN or Infinity values, and the ignoreNaNWindows property is set to false.
<table>
<tr><th>Input sequence length</th><th>Window size</th><th>Steps</th><th>Time taken (Mean)</th><th>Standard deviation</th></tr>
<tr><td>1000000</td><td>10</td><td>1</td><td>0.034561</td><td>0.001</td></tr>
<tr><td>1000000</td><td>10</td><td>10</td><td>0.03415</td><td>0.0005</td></tr>
<tr><td>1000000</td><td>100</td><td>1</td><td>0.0535</td><td>0.00185</td></tr>
<tr><td>1000000</td><td>100</td><td>100</td><td>0.052</td><td>0.001</td></tr>
<tr><td>1000000</td><td>1000</td><td>1</td><td>0.0622</td><td>0.0014</td></tr>
<tr><td>1000000</td><td>1000</td><td>1000</td><td>0.06063</td><td>0.0021</td></tr>
<tr><td>1000000</td><td>10000</td><td>1</td><td>0.07684</td><td>0.0011</td></tr>
<tr><td>1000000</td><td>10000</td><td>10000</td><td>0.07455</td><td>0.0018</td></tr>
</table>

#### Interpretation for big windows
***
The double-heap approach maintains the state of the current window while sliding over the input sequence.
As a result, each element of the input sequence enters the heap and remains there until it leaves the window, independent of the chosen step size.

In contrast, the tiny-window approach based on median/sorting networks processes each window independently.
It copies the complete window from the input sequence every *x* steps and computes the result without maintaining any state across windows. This makes the approach particularly efficient for very small window sizes.

For larger windows, however, copying the entire window becomes increasingly expensive.
In this case, the double-heap approach is more suitable, as it only removes one element and inserts one new element per window move.

This leads to the following implication:

- Increasing the step size does not reduce the runtime for large windows.
***

## Contact
If you have any questions, feel free to contact me:
jens.trappmann@icloud.com
