PYTHON = python2

all: wrapper so

wrapper: libxsmu_wrap.cxx

so:
	${PYTHON} setup.py build

install:
	${PYTHON} setup.py install

clean:
	rm -rf build libxsmu.py libxsmu_wrap.cxx *.o

libxsmu_wrap.cxx libxsmu.py: libxsmu.i
	swig -c++ -python -threads $<

.PHONY: all wrapper so install clean
