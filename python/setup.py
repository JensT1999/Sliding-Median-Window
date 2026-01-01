from setuptools import setup, Extension
from Cython.Build import cythonize
import numpy as np

extensions = [
    Extension(
        "sliding_median_window",
        sources=["sliding_median_window.pyx",
                "../src/medianwindow_api.c",
                "../src/median.c",
                "../src/tiny_medianwindow.c",
                "../src/median_window.c"],
        include_dirs=["../include", "../src", np.get_include()],
        language="c"
    )
]

setup(
    name="sliding_median_window",
    ext_modules=cythonize(extensions, compiler_directives={'language_level': "3"})
)
