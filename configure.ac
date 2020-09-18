# See semver.org for version info
#################################

m4_define([VERSION_MAJOR], [0])
m4_define([VERSION_MINOR], [1])
m4_define([VERSION_PATCH], [0])
m4_define([VERSION_STRING], VERSION_MAJOR.VERSION_MINOR.VERSION_PATCH)

# Init build tools
##################

AC_INIT([mnemo],[VERSION_STRING],[swann@anl.gov])
AC_CONFIG_SRCDIR([include/mnemo.h])
AC_CONFIG_AUX_DIR([m4])
AC_CONFIG_MACRO_DIR([m4])
# automake should fail on any error
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects 1.12])

# Detect features
#################

AC_LANG([C])
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_PROG_CPP
AC_TYPE_SIZE_T
AC_TYPE_INTPTR_T
AM_PROG_AR
LT_INIT

# Support for python package install
AM_PATH_PYTHON([3.0])

# Extra dependencies, configuration
###################################

AC_SUBST([PACKAGE_VERSION_MAJOR],[VERSION_MAJOR])
AC_SUBST([PACKAGE_VERSION_MINOR],[VERSION_MINOR])
AC_SUBST([PACKAGE_VERSION_PATCH],[VERSION_PATCH])

# support for testing with valgrind
###################################

AX_VALGRIND_DFLT([helgrind], [off])
AX_VALGRIND_DFLT([drd], [off])
AX_VALGRIND_DFLT([sgcheck], [off])
AX_VALGRIND_CHECK

# Support for cross-compiling check programs
AM_EXTRA_RECURSIVE_TARGETS([check-programs])

# Output
########


AC_CONFIG_HEADERS([include/config.h])

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 include/Makefile
		 tests/Makefile
		 bindings/Makefile
		 mnemo.pc
		 include/mnemo/utils/version.h])
AC_OUTPUT
