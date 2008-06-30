#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(eqntott, 9.0, pjscott@iastate.edu)
AM_INIT_AUTOMAKE(eqntott, 9.0)
AC_CONFIG_SRCDIR([src/hash.c])
AC_CONFIG_HEADER([src/config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h malloc.h stddef.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
                 src/Makefile
		 doc/Makefile])
AC_OUTPUT
