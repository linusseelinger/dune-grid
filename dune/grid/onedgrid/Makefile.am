noinst_LTLIBRARIES = libonedgrid.la

libonedgrid_la_SOURCES = onedgrid.cc onedgridfactory.cc nulliteratorfactory.cc
libonedgrid_la_LIBADD = $(DUNE_LIBS)

onedgriddir = $(includedir)/dune/grid/onedgrid/
onedgrid_HEADERS = nulliteratorfactory.hh  onedgridentity.hh \
   onedgridentitypointer.hh onedgridentityseed.hh onedgridfactory.hh onedgridgeometry.hh  onedgridhieriterator.hh \
   onedgridindexsets.hh  onedgridleafiterator.hh  onedgridleveliterator.hh \
   onedgridlist.hh  onedgridintersections.hh onedgridintersectioniterators.hh

include $(top_srcdir)/am/global-rules

EXTRA_DIST = CMakeLists.txt
