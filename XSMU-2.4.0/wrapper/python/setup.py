#!/usr/bin/env python

"""
setup.py file for XSMU
"""

from distutils.core import setup, Extension

libxsmu_module = Extension('_libxsmu',
    sources=['libxsmu_wrap.cxx', 'libxsmu.cxx'],
    library_dirs=['../../code/app/src'],
    libraries=['smu', 'ftdi'],
    extra_compile_args=['-std=c++11'],
    extra_link_args=['-std=c++11']
)

setup (name = 'libxsmu',
       version = '0.1',
       author      = "Krishnendu SWIG",
       description = """XPLORE SMU wrapper""",
       ext_modules = [libxsmu_module],
       py_modules = ["libxsmu"],
       )
