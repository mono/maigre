AC_PREREQ(2.52)
AC_INIT([maigre], [0.1])
AM_INIT_AUTOMAKE([1.9 dist-bzip2 tar-ustar foreign])
AM_MAINTAINER_MODE

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG

AM_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL

AC_PATH_PROG(MCS, gmcs)
if test x$MCS = x; then
	AC_MSG_ERROR([Could not find 'gmcs'])
fi

AC_PATH_PROG(MONO, mono)
if test x$MONO = x; then
	AC_MSG_ERROR([Could not find 'mono'])
fi

AC_PATH_PROG(PYTHON, python)
if test x$PYTHON = x; then
	AC_MSG_ERROR([Could not find 'python'])
fi

AC_MSG_CHECKING([for python mako])
if ! $PYTHON -c 'import mako' &>/dev/null; then
	AC_MSG_ERROR([not found; install the Python mako module (python-mako)])
fi
AC_MSG_RESULT([yes])

PKG_CHECK_MODULES(MONO_2_8, mono-2 >= 2.8,
	[have_mono_2_8=yes], [have_mono_2_8=no])
if test "x$have_mono_2_8" = xyes; then
	MONO_CFLAGS=$MONO_2_8_CFLAGS
	MONO_LIBS=$MONO_2_8_CFLAGS
else
	PKG_CHECK_MODULES(MONO_2_6, mono >= 2.6)
	MONO_CFLAGS=$MONO_2_6_CFLAGS
	MONO_LIBS=$MONO_2_6_LIBS
fi
AC_SUBST(MONO_CFLAGS)
AC_SUBST(MONO_LIBS)

PKG_CHECK_MODULES(GTK_SHARP, gtk-sharp-2.0 >= 2.12)
PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.12)

GTK_VERSION=`$PKG_CONFIG --variable=gtk_binary_version gtk+-2.0`
AC_SUBST(GTK_VERSION)

AC_CONFIG_FILES([
	Makefile
	maigre-codegen/Makefile
	libmaigre/Makefile
	Maigre/Makefile
])

AC_OUTPUT
