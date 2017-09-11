top_builddir = .
include ${top_builddir}/makeinclude

all:
	@echo "Please specify target: lib, wrapper, install, doc, or clean."

lib:
	${MAKE} -C code all

wrapper: lib
	$(MAKE) -C wrapper/python all

install: lib wrapper
	$(MAKE) -C code install
	$(MAKE) -C wrapper/python install

uninstall:
	rm -rf \
		$(INSTALL_INCLUDE_DIR) \
		$(INSTALL_LIB_DIR)/libsmu.a

doc:
	doxygen doxy.conf

clean:
	rm -f *~
	${MAKE} -C code clean
	$(MAKE) -C wrapper/python clean

.PHONY: all lib wrapper install doc clean
