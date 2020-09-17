AM_COLOR_TESTS = yes

AM_CFLAGS = -I$(top_srcdir)/include
AM_LDFLAGS = $(top_builddir)/src/libmnemo.la 

# valgrind support
@VALGRIND_CHECK_RULES@

# unit tests
UNIT_TESTS = 

# all tests
TST_PROGS = $(UNIT_TESTS)
check_PROGRAMS = $(TST_PROGS)
TESTS = $(TST_PROGS)

# phony target to allow us to compile the check programs without running the
# tests. Required for any environment where we are cross-compiling
check-programs-local: $(TST_PROGS)
