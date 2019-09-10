# ``XPLORE-Software``

### Introduction

**How this project fits into the scheme of things** :
- The ``XPLORE`` system is a sophisticated research grade tool that allows for the measurement of  electrical and magnetic transport properties of materials over a wide range of temperature. The system is divided into several modules (Fig.~\ref{fig:xplore_electronics}) described in the following sections, each of which allows the user to explore numerous important properties of their sample.

<!--- Add XPLORE image --->

- Each of XPLORE's modules has separate hardware/software

- Each complete module is made up of the following inter-related layers :
*Hardware* -> *Firmware* -> **Driver -> Application** -> *GUI*

**Functions of Software :**
- The software is C++ code that runs on the computer. The software can be categorized into :
    1. Driver : This is C++ code, that communicates with the firmware through an FTDI interface.
    2. Application Layer : This contains wrapper functions corresponding to each function in the Driver. `swig` is used to convert these C++ libraries into Python libraries, which further interface with the GUI layer.

**Structure of Software :**
- The software's code resides in the folder `ModuleName/code`. `ModuleName/code` further contains the following directories :

    1. `app`   : Application code for XPLORE module
    2. `stl`   : C++ standard template libraries
    3. `sys`   : Libraries for the hardware interface
    
`app` is further divided into `app` and `src`, where `app/app` contains all header (`.h`) files and `app/src` contains all `.cxx` files.

- The libraries reside in the folder `ModuleName/wrapper/python`. The C++ libraries are of the form `libModuleName.h' and `libModuleName.cxx`. `libModuleName.i` is an interface file for `swig`. From these files, `swig` generates a corresponding python library for the module, `libModuleName.py`. The instructions to generate the python library are given in the **Installation** section.

**Project Page :**

**Known Issues :** https://github.com/QuazarTech/XPLORE-firmware/issues

### Requirements

- XPLORE Physical Quantites Measurement System (Modules on which you want to modify software)

- Hardware requirements
  - Processor : Intel i3 or above
  - Storage : Atleast 10 GB
  - RAM : Atleast 4 GB

- OS requirements
  The XPLORE system has been tested on the following Operating Systems :
    - Ubuntu 16.04

- List of Dependencies:
  - python 2.7x
  - cmake
  - swig
  - avr-gcc toolchain
  
  Use the following command to get dependencies : 
  ```
  sudo apt-get install python cmake swig gcc-avr binutils-avr avr-libc gdb-avr avrdude
  ```
Found a missing dependency? Open an issue here : https://github.com/QuazarTech/XPLORE-software/issues

### Installation

1. Download the source code for this project using `git clone `
2. Navigate to folder `XPLORE-software/ModuleName/` (eg. `XPLORE-software/XSMU-2.4.0/`)
3. Run `make clean` to remove old dependency (`.dep`) files
4. Run `make wrapper` to generate the python library (`libModuleName.py`) file using swig.
5. Run `make install` to install the code in system directories. You might need to run this as `sudo`.
6. The software libraries for the XPLORE module have now been installed on your system. Each module's libraries will need to be installed separately using the above steps.

### Configuration


### Testing

Tests are provided for each function inside `XPLORE-software/ModuleName/wrapper/python/tests/`. To run the test :
    1. Navigate to `XPLORE-software/ModuleName/wrapper/python/tests/`
    2. Type `python testName.py` and press Enter.

### Troubleshooting and FAQ


### Maintainers

- Active :
  - Krishnendu Chatterjee (kc@quazartech.com)
  - Nishant Gupta (nishant@quazartech.com)
  - Gitansh Kataria (gitansh@quazartech.com)

- Past