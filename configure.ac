#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([icewm],[3.8.1],
        [https://github.com/bbidulock/icewm/issues],
        [icewm],[https://ice-wm.org])
AC_REVISION([3.8.1])

# set better defaults
AC_PREFIX_DEFAULT([/usr])
if test "$prefix" = /usr -o "$prefix" = NONE; then
    test "$sysconfdir" = '${prefix}/etc' && sysconfdir=/etc
    test "$sharedstatedir" = '${prefix}/com' && sharedstatedir=/var
    test "$localstatedir" = '${prefix}/var' && localstatedir=/var
fi

AC_CONFIG_SRCDIR([src/ysmapp.cc])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([scripts])
AC_USE_SYSTEM_EXTENSIONS
AC_CANONICAL_TARGET

DATE='2025-07-14'
AC_SUBST([DATE])

MDOCDATE='July 14, 2025'
AC_SUBST([MDOCDATE])

BRANCH='3.8.1'
AC_SUBST([BRANCH])

APPLICATIONS='icewm icewm-session icesh icewmhint icewmbg icehelp'
TESTCASES=`echo src/test*.cc | sed 's%src/\([[^ ]]*\)\.cc%\1%g'`
TESTCASES="$TESTCASES iceview icesame iceicon icerun icelist icebar" # iceclock
features=''

# Initialize Automake
AM_INIT_AUTOMAKE([gnits 1.12 dist-lzip no-dist-gzip std-options -Wall color-tests silent-rules tar-pax])
AM_MAINTAINER_MODE
AM_SILENT_RULES([yes])

# Checks for programs.
AC_LANG([C++])
AC_PROG_SED
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_PROG_AR
AC_PROG_MKDIR_P
AC_ARG_VAR([LD], [Linker loader command])

AC_ARG_VAR([MKFONTDIR],[mkfontdir command. @<:@default=mkfontdir@:>@])
AC_CHECK_PROGS([MKFONTDIR],[mkfontdir],[${am_missing_run}mkfontdir],[$PATH$PATH_SEPARATOR/usr/X11R6/bin])

AC_ARG_VAR([MARKDOWN],[markdown command. @<:@default=markdown@:>@])
AC_CHECK_PROGS([MARKDOWN],[markdown markdown_py discount-markdown Markdown.pl],
	       [${am_missing_run}markdown],[$PATH])

AC_ARG_VAR([ASCIIDOC],[asciidoc command. @<:@default=asciidoctor@:>@])
AC_CHECK_PROGS([ASCIIDOC],[asciidoc asciidoctor],[${am_missing_run}asciidoc],[$PATH])

AC_ARG_VAR([ASCIIDOC_BACKEND],[asciidoc backend @<:@default=NONE@:>@])
if test -z "$ASCIIDOC_BACKEND"; then
	case "$ASCIIDOC" in
		asciidoc) ASCIIDOC_BACKEND="--backend=html4" ;;
		asciidoctor) ASCIIDOC_BACKEND="--backend=html5" ;;
		*) AC_MSG_NOTICE([ASCIIDOC $ASCIIDOC is not supported.]) ;;
	esac
fi

AC_ARG_VAR([LINKS],[links command. @<:@default=lynx@:>@])
AC_CHECK_PROGS([LINKS],[lynx links],[${am_missing_run}lynx],[$PATH])
if test "$LINKS" = "links"; then
	LINKS="links -codepage utf-8"
fi

AC_DISABLE_STATIC
LT_PATH_LD
LT_PREREQ([2.4.2])
LT_INIT([dlopen])
LT_LANG([C++])
AC_SUBST([LIBTOOL_DEPS])

AM_GNU_GETTEXT_VERSION([0.21])
AM_GNU_GETTEXT([external])

AC_ARG_ENABLE([i18n],
    AS_HELP_STRING([--disable-i18n],[Disable internationalization.]))
if test x$enable_i18n != xno; then
    AC_DEFINE([CONFIG_I18N], [1], [Define to enable internationalization.])
    features="$features i18n"
    AM_ICONV
else
    unset LIBICONV LTLIBICONV
fi

AC_ARG_WITH([unicode-set],
    AS_HELP_STRING([--with-unicode-set=CODESET],[Your iconv unicode set in
	machine endian encoding (e.g. WCHAR_T, UCS-4-INTERNAL, UCS-4LE,
	UCS-4BE)]),
    [AC_DEFINE_UNQUOTED([CONFIG_UNICODE_SET],"$with_unicode_set",[Preferred unicode set.])])

PKG_PROG_PKG_CONFIG

# Checks for libraries.
AC_CHECK_FUNCS(clock_gettime, [], [AC_CHECK_LIB(rt, clock_gettime)])
case "${target_os}" in
    *solaris*)
        AC_CHECK_LIB(socket, socketpair)
        AC_CHECK_LIB(nsl, gethostbyname)
        ;;
    *openbsd*)
        test x$enable_i18n != xno -a -z "$EXTRA_LIBS" && EXTRA_LIBS=-liconv
        ;;
    *)
        :
        ;;
esac

# Checks for header files.
AS_BOX([Header Files])
AC_PATH_X
AC_PATH_XTRA
AC_CHECK_HEADERS([execinfo.h sched.h sys/sched.h])
AC_CHECK_HEADERS([sys/soundcard.h sys/sysctl.h uvm/uvm_param.h])

# Checks for typedefs, structures, and compiler characteristics.
AS_BOX([Typedefs, Structures, Compiler])
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_STRUCT_TM

AC_CHECK_SIZEOF(char, 1)
AC_CHECK_SIZEOF(short, 2)
AC_CHECK_SIZEOF(int, 4)
AC_CHECK_SIZEOF(long, 4)

# Checks for library functions.
if test x$cross_compling != xyes ; then :; AC_FUNC_FORK fi
if test x$cross_compling != xyes ; then :; AC_FUNC_MALLOC fi
if test x$cross_compling != xyes ; then :; AC_FUNC_REALLOC fi
AC_CHECK_FUNCS([backtrace_symbols_fd memrchr sysctl sysctlbyname \
                arc4random_uniform])
AC_FUNC_SELECT_ARGTYPES

AC_MSG_CHECKING([for strlcpy])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[#include <string.h>]],[[
               char a[2];
               if (strlcpy(a, "yes", 2) != 3)
                   return 1;]])],
              [
               AC_MSG_RESULT([yes])
               AC_DEFINE([HAVE_STRLCPY], 1,
                         [Define to 1 if you have the strlcpy function.])
              ],
              [
               AC_MSG_RESULT([no])
               AH_TEMPLATE([HAVE_STRLCPY],
                           [Define to 1 if you have the strlcpy function.])
              ])

AC_MSG_CHECKING([for strlcat])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[#include <string.h>]],[[
               char a[4] = "no";
               if (strlcat(a, "yes", 4) != 5)
                   return 1;]])],
              [
               AC_MSG_RESULT([yes])
               AC_DEFINE([HAVE_STRLCAT], 1,
                         [Define to 1 if you have the strlcat function.])
              ],
              [
               AC_MSG_RESULT([no])
               AH_TEMPLATE([HAVE_STRLCAT],
                           [Define to 1 if you have the strlcat function.])
              ])

AC_MSG_CHECKING([for C++11 by default])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[#include <regex>]],[[
               if (__cplusplus < 201103L) return 1;]])],
              [AC_MSG_RESULT([yes])],
              [AC_MSG_RESULT([no])
               AC_MSG_CHECKING([for C++11 when forced])
               CXXFLAGS="$CXXFLAGS -std=c++11"
               AC_RUN_IFELSE([AC_LANG_PROGRAM([[#include <regex>]],[[
                              if (__cplusplus < 201103L) return 1;]])],
                             [AC_MSG_RESULT([yes])],
                             [AC_MSG_RESULT([no])
                              AC_MSG_ERROR([Need C++11. Please set CXXFLAGS.])
                             ])
              ])

PKG_CHECK_MODULES([CORE],[fontconfig xrender xcomposite xdamage xfixes xext x11])
AC_SUBST([CORE_CFLAGS])
AC_SUBST([CORE_LIBS])

AC_CHECK_LIB([X11],[XInternAtoms],
    [AC_DEFINE([HAVE_XINTERNATOMS],[1],[Define to enable XInternAtoms])])

AC_ARG_ENABLE([sm],
    AS_HELP_STRING([--disable-sm],[Disable X session management.]))
if test x$enable_sm != xno ; then
    PKG_CHECK_MODULES([XSM],[sm ice],[
	AC_DEFINE([CONFIG_SESSION],[1],[Define to enable X session management])],
	[AC_MSG_ERROR([Packages SM and ICE are required for session mgmt.])])
fi

AC_ARG_ENABLE([shape],
    AS_HELP_STRING([--disable-shape],[Disable use of SHAPE extension.]))
if test x$enable_shape != xno ; then
    PKG_CHECK_MODULES([SHAPE],[xext],[
	CORE_CFLAGS="$SHAPE_CFLAGS $CORE_CFLAGS"
	CORE_LIBS="$SHAPE_LIBS $CORE_LIBS"
	AC_DEFINE([CONFIG_SHAPE],[1],[Define to enable X shape extension])
	features="$features shape"],
	[AC_MSG_ERROR([Package SHAPE is required for X Shape extension.])])
else
    AC_MSG_WARN([SHAPE disabled.])
fi

AC_ARG_ENABLE([xrandr],
    AS_HELP_STRING([--disable-xrandr],[Disable use of RANDR extension.]))
if test x$enable_xrandr != xno; then
    PKG_CHECK_MODULES([RANDR],[xrandr],[
	CORE_CFLAGS="$RANDR_CFLAGS $CORE_CFLAGS"
	CORE_LIBS="$RANDR_LIBS $CORE_LIBS"
	AC_DEFINE([CONFIG_XRANDR],[1],[Define to enable RANDR extension.])
	features="$features xrandr"],
	[AC_MSG_ERROR([Package XRANDR is required for X RandR extension.])])
else
    AC_MSG_WARN([RANDR disabled.])
fi

AC_ARG_ENABLE([xres],
    AS_HELP_STRING([--disable-xres],[Disable use of XRES extension.]))
if test x$enable_xres != xno; then
    PKG_CHECK_MODULES([XRES],[xres >= 1.2],[
	CORE_CFLAGS="$XRES_CFLAGS $CORE_CFLAGS"
	CORE_LIBS="$XRES_LIBS $CORE_LIBS"
	AC_DEFINE([HAVE_XRES],[1],[Define 1 if libXRes has XResQueryClientIds])
	features="$features xres"],
	[AC_MSG_NOTICE([Package XRES is required for X-resource extension.])])
else
    AC_MSG_WARN([XRES disabled.])
fi

AC_ARG_ENABLE([xfreetype],
    AS_HELP_STRING([--disable-xfreetype],[Disable use of XFT for text rendering.]))
if test x$enable_xfreetype != xno; then
    PKG_CHECK_MODULES([XFT2],[xft >= 2.0.0],[
	CORE_CFLAGS="$XFT2_CFLAGS $CORE_CFLAGS"
	CORE_LIBS="$XFT2_LIBS $CORE_LIBS"
	AC_DEFINE([CONFIG_XFREETYPE],[2],[Define to enable XFT support.])
	features="$features xfreetype"],
	[AC_MSG_ERROR([Package XFT 2+ is required for X FreeType rendering.])
        enable_corefonts=yes])
else
    enable_corefonts=yes
fi

AC_ARG_ENABLE([corefonts],
    AS_HELP_STRING([--enable-corefonts],[Support X11 core fonts.]))
if test x$enable_corefonts = xyes; then
    AC_DEFINE([CONFIG_COREFONTS],[1],[Define to enable X11 core fonts.])
    features="$features corefonts"
fi

if  test x"${features#*xfreetype}" = x"$features" &&
    test x"${features#*corefonts}" = x"$features"
then
    AC_MSG_ERROR([Either xfreetype or corefonts must be enabled])
fi

AC_ARG_WITH([icesound],
    AS_HELP_STRING([--with-icesound=INTERFACES],[List of audio interfaces for icesound. @<:@default: ALSA,AO,OSS@:>@]),[],
    [with_icesound='ALSA,AO,OSS'])

audio_support=
for iface in `echo "$with_icesound"|sed 's|,| |g'` ; do
    case $iface in
    (ALSA|alsa)
        PKG_CHECK_MODULES([ALSA],[sndfile alsa],[
            AUDIO_CFLAGS="$AUDIO_CFLAGS $ALSA_CFLAGS"
            AUDIO_LIBS="$AUDIO_LIBS $ALSA_LIBS"
            AC_DEFINE([ENABLE_ALSA],[1],[Define to enable ALSA support.])
            CONFIG_ALSA=yes; audio_support=$iface; break],
            [AC_MSG_WARN([ALSA is not supported])])
        ;;
    (AO|ao)
        PKG_CHECK_MODULES([AO],[sndfile ao],[
            AUDIO_CFLAGS="$AUDIO_CFLAGS $AO_CFLAGS"
            AUDIO_LIBS="$AUDIO_LIBS $AO_LIBS"
            AC_DEFINE([ENABLE_AO],[1],[Define to enable AO support.])
            CONFIG_AO=yes; audio_support=$iface; break],
            [AC_MSG_WARN([AO is not supported])])
        ;;
    (OSS|oss)
        PKG_CHECK_MODULES([OSS],[sndfile],[
            AUDIO_CFLAGS="$AUDIO_CFLAGS $OSS_CFLAGS"
            AUDIO_LIBS="$AUDIO_LIBS $OSS_LIBS"
            AC_DEFINE([ENABLE_OSS],[1],[Define to enable OSS support.])
            CONFIG_OSS=yes; audio_support=$iface; break],
            [AC_MSG_WARN([OSS is not supported])])
        ;;
    (no)
        true
        ;;
    (*)
        AC_MSG_WARN([Invalid audio interface: $iface])
        ;;
    esac
done
if test "x$CONFIG_ALSA" = "xyes" ||
   test "x$CONFIG_AO"   = "xyes" ||
   test "x$CONFIG_OSS"  = "xyes"
then
    BUILD_SOUND=yes
    APPLICATIONS="${APPLICATIONS} icesound"
else
    audio_support=no
    if test "x$with_icesound" != "xno"
    then
        AC_MSG_WARN([No valid sound interface, not building icesound.])
    fi
fi

AC_SUBST([AUDIO_CFLAGS])
AC_SUBST([AUDIO_LIBS])

AC_ARG_ENABLE([xinerama],
    AS_HELP_STRING([--disable-xinerama],[Disable XINERAMA support.]))
if test x$enable_xinerama != xno ; then
    PKG_CHECK_MODULES([XINERAMA],[xinerama],[
	CORE_CFLAGS="$XINERAMA_CFLAGS $CORE_CFLAGS"
	CORE_LIBS="$XINERAMA_LIBS $CORE_LIBS"
	AC_DEFINE([XINERAMA],[1],[Define to enable Xinerama support.])
	features="$features xinerama"],
	[AC_MSG_ERROR([Package XINERAMA is required for Xinerama extension.])])
fi

AC_ARG_ENABLE([fribidi],
    AS_HELP_STRING([--disable-fribidi],[Disable right to left support.]))
if test "$enable_fribidi" != "no" && test "$enable_i18n" != "no"; then
    PKG_CHECK_MODULES([FRIBIDI], [fribidi],[
	CORE_CFLAGS="$FRIBIDI_CFLAGS $CORE_CFLAGS"
	CORE_LIBS="$FRIBIDI_LIBS $CORE_LIBS"
	AC_DEFINE([CONFIG_FRIBIDI],[1],[Define to enable fribidi support.])
	features="$features fribidi"],
	[AC_MSG_ERROR([Package FRIBIDI is required for FriBidi support.])])
fi

AC_ARG_ENABLE([menus-fdo],
    AS_HELP_STRING([--disable-menus-fdo],[Disable support of freedesktop.org style menus.]))
if test x$enable_menus_fdo != xno; then
PKG_CHECK_MODULES([GIO],[gio-2.0 gio-unix-2.0],[
	AC_DEFINE([CONFIG_FDO_MENUS],[1],[Define to support freedesktop.org style menus.])
	BUILD_MENU_FDO=yes
	APPLICATIONS="${APPLICATIONS} icewm-menu-fdo"],
	[AC_MSG_WARN([Freedesktop style menus not supported.])])
fi
AC_SUBST([GIO_CFLAGS])
AC_SUBST([GIO_LIBS])

AC_ARG_ENABLE([librsvg],
    AS_HELP_STRING([--disable-librsvg],[Disable SVG using librsvg.]))
AC_ARG_ENABLE([imlib2],
    AS_HELP_STRING([--disable-imlib2],[Disable imlib2 rendering.
        @<:@default=enabled@:>@]))
AC_ARG_ENABLE([nanosvg],
    AS_HELP_STRING([--enable-nanosvg],[Enable SVG using nanosvg.]))
AC_ARG_ENABLE([gdk-pixbuf],
    AS_HELP_STRING([--enable-gdk-pixbuf],[Enable gdk-pixbuf rendering.
	@<:@default=disabled@:>@]))

image_library=
if test "x$enable_gdk_pixbuf" = xyes
then
    PKG_CHECK_MODULES([PIXBUF],[gdk-pixbuf-xlib-2.0],[
	IMAGE_CFLAGS="$PIXBUF_CFLAGS"
	IMAGE_LIBS="$PIXBUF_LIBS"
	AC_DEFINE([CONFIG_GDK_PIXBUF_XLIB],[1],
                  [Define to enable gdk-pixbuf rendering.])
	image_library="gdk-pixbuf-xlib-2.0"])
fi

if test "x$enable_gdk_pixbuf" != xyes &&
   test "x$enable_imlib2" != xno
then
    PKG_CHECK_MODULES([IMLIB2],[imlib2],[
	IMAGE_CFLAGS="$IMLIB2_CFLAGS"
	IMAGE_LIBS="$IMLIB2_LIBS"
	AC_DEFINE([CONFIG_IMLIB2],[1],[Define to enable imlib2 rendering.])
	image_library="imlib2"])
fi

if test "x$image_library" = "xgdk-pixbuf-xlib-2.0" ||
   test "x$image_library" = "ximlib2"
then
    if test "x$enable_nanosvg" = xyes
    then
        if ! test -f src/nanosvg.h ||
           ! test -s src/nanosvg.h ||
           ! test -f src/nanosvgrast.h ||
           ! test -s src/nanosvgrast.h
        then
            hub=https://raw.githubusercontent.com/memononen/nanosvg/master/src/
            if command -v wget >/dev/null
            then
                AC_MSG_NOTICE([downloading nanosvg using wget])
                if ! wget -q --timeout=3 -e robots=off -e noclobber=off \
                    -P src $hub/nanosvg.h $hub/nanosvgrast.h
                then
                    AC_MSG_WARN([Download of nanosvg failed.])
                fi
            fi
        fi
        if test -f src/nanosvg.h &&
           test -s src/nanosvg.h &&
           test -f src/nanosvgrast.h &&
           test -s src/nanosvgrast.h
        then
            AC_DEFINE([CONFIG_NANOSVG],[1],[Define to enable nanosvg rendering.])
        else
            AC_MSG_WARN([Nanosvg includes are missing. SVG is not supported.])
        fi
    fi

    if test "x$enable_nanosvg" != xyes &&
       test "x$enable_librsvg" != xno
    then
        PKG_CHECK_MODULES([LIBRSVG],[librsvg-2.0],[
            IMAGE_CFLAGS="$LIBRSVG_CFLAGS $IMAGE_CFLAGS"
            IMAGE_LIBS="$LIBRSVG_LIBS $IMAGE_LIBS"
            AC_DEFINE([CONFIG_LIBRSVG],[1],[Define to enable librsvg rendering.])
            image_library="$image_library librsvg-2.0"],
            [AC_MSG_WARN([Package librsvg is missing. SVG is not supported.])])
    fi

    PKG_CHECK_MODULES([XPM],[xpm],[
	IMAGE_CFLAGS="$IMAGE_CFLAGS $XPM_CFLAGS"
	IMAGE_LIBS="$IMAGE_LIBS $XPM_LIBS"
	AC_DEFINE([CONFIG_XPM],[1],[Define to enable libxpm rendering.])
	image_library="$image_library libxpm"],
	[AC_MSG_ERROR([Package libXpm is required for themable cursors.])])
fi

if test "x$enable_gdk_pixbuf" != xyes &&
   test "x$enable_imlib2" = xno
then
    PKG_CHECK_MODULES([XPM],[xpm],[
	IMAGE_CFLAGS="$XPM_CFLAGS"
	IMAGE_LIBS="$XPM_LIBS"
	AC_DEFINE([CONFIG_XPM],[1],[Define to enable libxpm rendering.])
	image_library=libxpm],
	[AC_MSG_ERROR([Package libXpm is required for themable cursors.])])
    PKG_CHECK_MODULES([LIBPNG],[libpng >= 1.2],[
	IMAGE_CFLAGS="$LIBPNG_CFLAGS $IMAGE_CFLAGS"
	IMAGE_LIBS="$LIBPNG_LIBS $IMAGE_LIBS"
	AC_DEFINE([CONFIG_LIBPNG],[1],[Define to enable libpng rendering.])
	image_library="$image_library libpng"],
	[AC_MSG_ERROR([Package libpng is required to load icewm themes.])])
    PKG_CHECK_MODULES([LIBJPEG],[libjpeg],[
        IMAGE_CFLAGS="$LIBJPEG_CFLAGS $IMAGE_CFLAGS"
        IMAGE_LIBS="$LIBJPEG_LIBS $IMAGE_LIBS"
        AC_DEFINE([CONFIG_LIBJPEG],[1],[Define to enable libjpeg rendering.])
        image_library="$image_library libjpeg"],
        [if test -f /usr/include/jpeglib.h &&
            test -x /usr/lib64/libjpeg.so ; then
            IMAGE_LIBS="-ljpeg $IMAGE_LIBS"
            AC_DEFINE([CONFIG_LIBJPEG],[1],[Define to enable libjpeg rendering.])
        fi])
fi
AC_SUBST([IMAGE_CFLAGS])
AC_SUBST([IMAGE_LIBS])

AM_CONDITIONAL([BUILD_TESTS],[test x$BUILD_TESTS = xyes])
AM_CONDITIONAL([BUILD_SOUND],[test x$BUILD_SOUND = xyes])
AM_CONDITIONAL([BUILD_MENU_FDO],[test x$BUILD_MENU_FDO = xyes])
AM_CONDITIONAL([BUILD_ICEWMTRAY],[test x$BUILD_ICEWMTRAY = xyes])

test "x$prefix" = xNONE && prefix="$ac_default_prefix"

AC_ARG_WITH([libdir],
    AS_HELP_STRING([--with-libdir=PATH],[Package data directory.
	@<:@default=DATADIR/icewm@:>@]))
if test -z "$with_libdir" -o "x$with_libdir" = xyes ; then
    with_libdir='${datadir}/icewm'
fi
ICE_EXPAND([LIBDIR],[${with_libdir}])
AC_DEFINE_UNQUOTED([LIBDIR],["${LIBDIR}"],
	[Define to package data directory.])
AC_SUBST([LIBDIR])

AC_ARG_WITH([cfgdir],
    AS_HELP_STRING([--with-cfgdir=PATH],[Package configuration directory.
	@<:@default=SYSCONFDIR/icewm@:>@]))
if test -z "$with_cfgdir" -o "x$with_cfgdir" = xyes ; then
    with_cfgdir='${sysconfdir}/icewm'
fi
ICE_EXPAND([CFGDIR],[${with_cfgdir}])
AC_DEFINE_UNQUOTED([CFGDIR],["${CFGDIR}"],
	[Define to package configuration directory.])
AC_SUBST([CFGDIR])

AC_ARG_WITH([docdir],
    AS_HELP_STRING([--with-docdir=PATH],[Package documentation directory.
	@<:@default=DOCDIR@:>@]))
if test -z "$with_docdir" -o "x$with_docdir" = xyes ; then
    with_docdir='${docdir}'
fi
ICE_EXPAND([DOCDIR],[${with_docdir}])
AC_DEFINE_UNQUOTED([DOCDIR],["${DOCDIR}"],
	[Define to package documentation directory.])

ICE_EXPAND([LOCDIR],[${localedir}])
AC_DEFINE_UNQUOTED([LOCDIR],["${LOCDIR}"],
	[Define to locale directory.])

AC_ARG_WITH([theme],
    AS_HELP_STRING([--with-theme=NAME],[Default theme.
                    [default="icedesert/default.theme"]]),,
                    with_theme="icedesert/default.theme")
AC_DEFINE_UNQUOTED([CONFIG_DEFAULT_THEME],
                   "${with_theme}",[Name of default theme])
AC_ARG_WITH([background],
    AS_HELP_STRING([--with-background=PATH],
                   [Default background image or directory.]),
                   [],[with_background=""])
AC_DEFINE_UNQUOTED([CONFIG_DEFAULT_BACKGROUND], "${with_background}",
                   [Path of default background image or directory.])
AC_ARG_WITH([xterm],
    AS_HELP_STRING([--with-xterm=xterm],[Default X11 terminal emulator.]),
        [],[with_xterm=xterm])
AC_SUBST([XTERMCMD],[${with_xterm}])
AC_DEFINE_UNQUOTED([XTERMCMD],[${with_xterm}],[Name of terminal emulator.])

AC_DEFINE_UNQUOTED([HOSTOS],["${host_os}"],
	[Define to the host operating system.])
AC_DEFINE_UNQUOTED([HOSTCPU],["${host_cpu}"],
	[Define to the host CPU.])
AC_DEFINE_UNQUOTED([ICEHELPIDX],["${DOCDIR}/icewm.html"],
	[Define to the IceWM help index file.])

AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug],[Enable debugging of IceWM.]))
if test x$enable_debug = xyes ; then
    AC_DEFINE([DEBUG],[1],[Define if you want to debug IceWM.])
    CXXFLAGS="$CXXFLAGS -g -DDEBUG"
    features="$features debugging-symbols"
fi
AC_ARG_ENABLE([logevents],
    AS_HELP_STRING([--enable-logevents],[Support logging of X11 events.]))
if test x$enable_logevents = xyes ; then
    AC_DEFINE([LOGEVENTS],[1],[Define to enable X11 event logging support.])
    features="$features logevents"
fi

CORE_LIBS="$CORE_LIBS -lm"

AC_ARG_VAR([EXTRA_LIBS], [Extra core libraries.])

CORE_LIBS="$CORE_LIBS "'$(EXTRA_LIBS)'

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 po/Makefile.in
                 doc/Makefile
		 man/Makefile
                 lib/Makefile
		 lib/keys
		 lib/menu
		 lib/programs
		 lib/toolbar
		 lib/winoptions])
AC_OUTPUT

# Display results
AC_MSG_RESULT([])
ICE_MSG_VALUE([Applications],	APPLICATIONS)
ICE_MSG_VALUE([Image library],	image_library)
ICE_MSG_VALUE([Audio support],	audio_support)
ICE_MSG_VALUE([Features],	features)
ICE_MSG_VALUE([Paths: PREFIX], prefix)
ICE_MSG_VALUE([       BINDIR], bindir)
ICE_MSG_VALUE([       LOCDIR], localedir)
ICE_MSG_VALUE([       LIBDIR], LIBDIR)
ICE_MSG_VALUE([       CFGDIR], CFGDIR)
ICE_MSG_VALUE([       DOCDIR], DOCDIR)
ICE_MSG_VALUE([       MANDIR], mandir)
ICE_MSG_VALUE([       XTERMCMD], XTERMCMD)
AC_MSG_RESULT([])

# vim: set ft=config sw=4:
