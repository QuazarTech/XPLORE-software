top_builddir = ../..
include ${top_builddir}/makeinclude

all:
	${MAKE} -C src dep
	${MAKE} -C src obj
	${MAKE} -C src lib

install:
	mkdir -p $(INSTALL_INCLUDE_DIR)/app
	cp -r ./app $(INSTALL_INCLUDE_DIR)/app

	mkdir -p $(INSTALL_LIB_DIR)
	cp -r ./src/libsmu.a $(INSTALL_LIB_DIR)

clean:
	${MAKE} -C src clean
	rm -rf *~ */*~
