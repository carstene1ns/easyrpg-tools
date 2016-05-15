AC_INIT([encdetect],[1.0],[https://github.com/EasyRPG/Tools/issues],[encdetect],[https://easy-rpg.org/])

AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])
AM_SILENT_RULES([yes])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_LANG([C++])
AC_PROG_CXX

AX_CXX_COMPILE_STDCXX(11, noext)
PKG_CHECK_MODULES([LCF], [liblcf])
# optional libs
AC_ARG_WITH([icu],[AS_HELP_STRING([--without-icu],
	[Disable ICU @<:@default=auto@:>@])])
AS_IF([test "x$with_icu" != "xno"],[
	PKG_CHECK_MODULES([ICU], [icu-i18n],[AC_DEFINE(HAVE_ICU,[1],[icu support])],[auto_icu=0])
])
AC_ARG_WITH([guess],[AS_HELP_STRING([--without-guess],
	[Disable libguess @<:@default=auto@:>@])])
AS_IF([test "x$with_guess" != "xno"],[
	PKG_CHECK_MODULES([GUESS], [libguess],[AC_DEFINE(HAVE_GUESS,[1],[libguess support])],[auto_guess=0])
])
AC_ARG_WITH([enca],[AS_HELP_STRING([--without-enca],
	[Disable ENCA @<:@default=auto@:>@])])
AS_IF([test "x$with_enca" != "xno"],[
	PKG_CHECK_MODULES([ENCA], [enca],[AC_DEFINE(HAVE_ENCA,[1],[ENCA support])],[auto_enca=0])
])

AC_OUTPUT
