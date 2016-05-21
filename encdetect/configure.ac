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
	PKG_CHECK_MODULES([ICU], [icu-i18n],[AC_DEFINE(HAVE_ICU,[1],[ICU support])],
		[AC_MSG_WARN([ICU not found, support disabled])])
],[
	AC_MSG_NOTICE([ICU support disabled])
])
AC_ARG_WITH([uchardet],[AS_HELP_STRING([--without-uchardet],
	[Disable uchardet @<:@default=auto@:>@])])
AS_IF([test "x$with_uchardet" != "xno"],[
	PKG_CHECK_MODULES([UCHARDET], [uchardet],[AC_DEFINE(HAVE_UCHARDET,[1],[uchardet support])],
		[AC_MSG_WARN([uchardet not found, support disabled])])
],[
	AC_MSG_NOTICE([uchardet support disabled])
])

AC_OUTPUT
