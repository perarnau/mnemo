AM_CPPFLAGS = -I$(top_srcdir)/include

#############################################
# valgrind support
@VALGRIND_CHECK_RULES@

#############################################
# .C sources

REUSE_SOURCES = reuse.c

LIB_SOURCES = \
	      $(REUSE_SOURCES) \
	      mnemo.c

lib_LTLIBRARIES = libmnemo.la

libmnemo_la_SOURCES=$(LIB_SOURCES)
