SUBDIRS = \
  albertagrid \
  alugrid \
  common \
  geometrygrid \
  identitygrid \
  io \
  onedgrid \
  sgrid \
  test \
  uggrid \
  utility \
  yaspgrid
noinst_LTLIBRARIES = libgrid.la

# always part of lib:
ONEDLIB = onedgrid/libonedgrid.la
DGFPARSERLIB = io/file/dgfparser/libdgfparser.la

# conditional parts:
if UG
  UGLIB = uggrid/libuggrid.la
endif

if ALUGRID
  ALULIB = alugrid/libalu.la
endif

# construct only from sub-libraries
libgrid_la_SOURCES =
# This forces automake to use the C++ linker
# (see the automake manual, section "Libtool Convenience Libraries")
nodist_EXTRA_libgrid_la_SOURCES = dummy.cc
sourcescheck_DUMMY = dummy.cc
libgrid_la_LIBADD = $(ONEDLIB) $(UGLIB) $(ALULIB) \
	$(DGFPARSERLIB)				\
        $(AMIRAMESHLIB)                         \
	$(DUNE_LIBS)

# These libraries do not need other libraries besides libdunecommon and libdunegeometry
griddir = $(includedir)/dune/grid
grid_HEADERS = albertagrid.hh \
               alugrid.hh \
               identitygrid.hh \
               sgrid.hh \
               uggrid.hh \
               yaspgrid.hh \
               onedgrid.hh \
	       geometrygrid.hh

include $(top_srcdir)/am/global-rules

EXTRA_DIST = CMakeLists.txt
