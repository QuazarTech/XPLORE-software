top_builddir = ../..
include ${top_builddir}/makeinclude

all:
	${MAKE} -C src dep
	${MAKE} -C src obj
	${MAKE} -C src lib

install:
	mkdir -p $(INSTALL_INCLUDE_DIR)/sys
	cp -r ./sys $(INSTALL_INCLUDE_DIR)/sys

clean:
	${MAKE} -C src clean
	rm -rf *~ */*~
