# which tests to run
TESTS =

# programs just to build when "make check" is used
check_PROGRAMS =

# define the programs
TESTS += globalindexsettest
check_PROGRAMS += globalindexsettest
globalindexsettest_SOURCES = globalindexsettest.cc
globalindexsettest_CPPFLAGS = $(AM_CPPFLAGS) \
                              $(DUNEMPICPPFLAGS) \
	                      $(UG_CPPFLAGS)
globalindexsettest_LDFLAGS = $(AM_LDFLAGS) \
                             $(DUNEMPILDFLAGS) \
	                     $(UG_LDFLAGS)
globalindexsettest_LDADD = $(UG_LIBS) $(DUNEMPILIBS) \
                           $(LDADD)

# define the programs
TESTS += structuredgridfactorytest
check_PROGRAMS += structuredgridfactorytest
structuredgridfactorytest_SOURCES = structuredgridfactorytest.cc
structuredgridfactorytest_CPPFLAGS = $(AM_CPPFLAGS) \
	                            $(DUNEMPICPPFLAGS) \
	                            $(UG_CPPFLAGS)
structuredgridfactorytest_LDFLAGS = $(AM_LDFLAGS) \
	                            $(DUNEMPILDFLAGS) \
	                            $(UG_LDFLAGS)
structuredgridfactorytest_LDADD = $(UG_LIBS) \
	                            $(DUNEMPILIBS) \
                                $(LDADD)

TESTS += tensorgridfactorytest
check_PROGRAMS += tensorgridfactorytest
tensorgridfactorytest_SOURCES = tensorgridfactorytest.cc
tensorgridfactorytest_CPPFLAGS = $(AM_CPPFLAGS) \
	                            $(DUNEMPICPPFLAGS) \
	                            $(UG_CPPFLAGS)
tensorgridfactorytest_LDFLAGS = $(AM_LDFLAGS) \
	                            $(DUNEMPILDFLAGS) \
	                            $(UG_LDFLAGS)
tensorgridfactorytest_LDADD = $(UG_LIBS) \
	                            $(DUNEMPILIBS) \
                                $(LDADD)


TESTS += vertexordertest
check_PROGRAMS += vertexordertest
vertexordertest_SOURCES = vertexordertest.cc
vertexordertest_CPPFLAGS = $(AM_CPPFLAGS)	\
	$(ALUGRID_CPPFLAGS)			\
	$(UG_CPPFLAGS)
vertexordertest_LDFLAGS = $(AM_LDFLAGS)		\
	$(ALUGRID_LDFLAGS)			\
	$(UG_LDFLAGS)
vertexordertest_LDADD =				\
	$(UG_LIBS)				\
	$(ALUGRID_LIBS)				\
	$(LDADD)

TESTS += persistentcontainertest
check_PROGRAMS += persistentcontainertest
persistentcontainertest_SOURCES = persistentcontainertest.cc
persistentcontainertest_CPPFLAGS = $(AM_CPPFLAGS)	\
	$(ALUGRID_CPPFLAGS)			\
	$(UG_CPPFLAGS)
persistentcontainertest_LDFLAGS = $(AM_LDFLAGS)		\
	$(ALUGRID_LDFLAGS)			\
	$(UG_LDFLAGS)
persistentcontainertest_LDADD =				\
	$(UG_LIBS)				\
	$(ALUGRID_LIBS)				\
	$(LDADD)

include $(top_srcdir)/am/global-rules

EXTRA_DIST = CMakeLists.txt
