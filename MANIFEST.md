title: GR fosphor
brief: GNU Radio block for RTSA-like spectrum visualization using OpenCL and OpenGL acceleration
tags:
    - fft
    - gpu
    - opencl
    - opengl
    - visualization
    - osmocom
author:
    - Sylvain Munaut <tnt@246tNt.com>
copyright_owner:
    - Sylvain Munaut <tnt@246tNt.com>
license: GPLv3
repo: git://git.osmocom.org/gr-fosphor
website: http://sdr.osmocom.org/trac/wiki/fosphor
icon: http://people.osmocom.org/~tnt/stuff/fosphor-icon.png
---
GNU Radio block for RTSA-like spectrum visualization using OpenCL and OpenGL acceleration

A direct integration into `osmocom_fft` is available through the -F option.

![LTE](http://sdr.osmocom.org/trac/raw-attachment/wiki/GrOsmoSDR/fosphor.png "LTE")
![GSM](http://sdr.osmocom.org/trac/raw-attachment/wiki/GrOsmoSDR/fosphor2.png "GSM")

It requires a working OpenCL driver, either CPU or GPU, to be installed.
Please refer to the documentation on http://sdr.osmocom.org/trac/wiki/fosphor
for installation instructions.
