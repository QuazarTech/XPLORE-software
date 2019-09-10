top_builddir = ../../..
include ${top_builddir}/makeinclude

CPP_SRC = \
	Comm.cxx \
	CS.cxx \
	VS.cxx \
	CM.cxx \
	VM.cxx \
	VM2.cxx \
	virtuaSMU.cxx \
	SystemConfig.cxx \
	version.cxx \
	Exception.cxx \

DEP = ${CPP_SRC:%.cxx=%.dep}
OBJ = ${CPP_SRC:%.cxx=%.o}
LOBJ = ${CPP_SRC:%.cxx=%.lo}

ifeq (${MAKECMDGOALS}, obj)
include ${DEP}
endif

dep: ${DEP}

obj: ${LOBJ}

lib: libapp.la libsmu.a

libapp.la: $(LOBJ)
	$(LINK) $(LOBJ) -o $@

libsmu.a:
	$(LINK) $(LIBS) -o $@

clean:
	$(RM) $(DEP) $(OBJ) $(LOBJ) libapp.la libsmu.a .libs test main *~

.PHONY: dep obj lib clean
