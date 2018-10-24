import os, sys

from distutils.core import setup, Extension
from distutils.command import build_ext


py_ver = sys.version_info[:2]

include_dirs = ["/usr/local/include"]
library_dirs = ["/usr/local/lib"]
libraries = ["boost_python-py{}{}".format(*py_ver), "stdc++"]
sources = ["src/boost_lagrange.cpp"]

lagrange = Extension(
    name="lagrange._lagrange",
    include_dirs=include_dirs,
    libraries=libraries,
    library_dirs=library_dirs,
    sources=sources,
    extra_compile_args=['-std=c++11']
)

setup(
    name="lagrange",
    version="1.0.1",
    description="C++ implementation of scipy.interpolate.lagrange",
    author="mugwort_rc",
    author_email="mugwort rc at gmail com",
    url="https://github.com/mugwort-rc/boost_python_lagrange",
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: BSD License",
        "Programming Language :: C++",
        "Programming Language :: Python",
        "Programming Language :: Python :: 2.7",
        "Programming Language :: Python :: 3.5",
    ],
    license="BSD",
    packages=["lagrange"],
    ext_modules=[lagrange],
)

