
########
########    WARNING: DO NOT CHANGE THIS FILE WITHOUT RUNNING dist.sh SUCCESSFULLY
########    BEFORE PUSHING YOUR CHANGES!
########


AM_CPPFLAGS = -include ../config.h
AM_CXXFLAGS = $(CORE_CFLAGS) $(XSM_CFLAGS) $(IMAGE_CFLAGS) $(AUDIO_CFLAGS) $(GIO_CFLAGS) -DEXEEXT=$(EXEEXT)

EXTRA_DIST = \
	ypointer.h \
	globit.c

EXTRA_PROGRAMS = \
	icehelp \
	icesound \
	icewm-menu-fdo \
	testarray \
	testlocale \
	testmap \
	testmenus \
	testnetwmhints \
	testpointer \
	testwinhints \
	iceview \
	icesame \
	iceicon \
	icerun \
	icelist \
	strtest \
	iceskt \
	iceclock

noinst_LTLIBRARIES = \
	libice.la\
	libitk.la

bin_PROGRAMS = \
	icewm \
	icewm-session \
	icesh \
	icehelp \
	icewmhint \
	icewmbg

noinst_PROGRAMS = \
	genpref

TESTS = strtest testpointer testarray

if BUILD_TESTS
noinst_PROGRAMS += \
	testarray \
	testlocale \
	testmap \
	testmenus \
	testnetwmhints \
	testpointer \
	testwinhints \
	iceview \
	icesame \
	iceicon \
	icerun \
	icelist \
	strtest
endif

if BUILD_SOUND
bin_PROGRAMS += icesound
endif

if BUILD_MENU_FDO
bin_PROGRAMS += icewm-menu-fdo
endif

if BUILD_ICEWMTRAY
bin_PROGRAMS += icewmtray
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT = icewmtray
endif

libice_la_SOURCES = \
	MwmUtil.h \
	WinMgr.h \
	argument.h \
	ascii.h \
	base.h \
	debug.h \
	default.h \
	guievent.h \
	intl.h \
	keysyms.cc \
	keysyms.h \
	logevent.cc \
	logevent.h \
	misc.cc \
	mstring.cc \
	mstring.h \
	objmenu.cc \
	objmenu.h \
	prefs.h \
	ref.cc \
	ref.h \
	sysdep.h \
	themable.h \
	udir.cc \
	udir.h \
	upath.cc \
	upath.h \
	wmkey.cc \
	wmkey.h \
	wmmgr.h \
	wmprog.h \
	workspaces.h \
	yaction.h \
	yapp.cc \
	yapp.h \
	yarray.cc \
	yarray.h \
	ybidi.h \
	ycolor.cc \
	ycolor.h \
	yconfig.cc \
	yconfig.h \
	ycursor.cc \
	ycursor.h \
	yfileio.cc \
	yfileio.h \
	yfont.cc \
	yfontbase.h \
	yfontcache.h \
	yfontcore.cc \
	fontmacro.h \
	yfontname.h \
	yfontxft.cc \
	yfull.h \
	yimage.h \
	yimage2.cc \
	yimage2.h \
	yimage_gdk.cc \
	yimage_gdk.h \
	ykey.h \
	ylayout.h \
	ylib.h \
	ylist.h \
	ylocale.cc \
	ylocale.h \
	ymenu.h \
	ymsgbox.h \
	ypaint.cc \
	ypaint.h \
	ypipereader.cc \
	ypipereader.h \
	ypixmap.cc \
	ypixmap.h \
	ypointer.h \
	ypoll.h \
	ypopup.cc \
	ypopup.h \
	yprefs.cc \
	yprefs.h \
	yrect.h \
	ysocket.cc \
	ysocket.h \
	ystring.cc \
	ystring.h \
	ysvg.cc \
	ytime.cc \
	ytime.h \
	ytimer.cc \
	ytimer.h \
	ytooltip.cc \
	ytooltip.h \
	ytrace.h \
	ywindow.cc \
	ywindow.h \
	ywordexp.h \
	yxapp.cc \
	yxapp.h \
	yxcontext.h \
	yxembed.cc \
	yxembed.h \
	yximage.cc

libitk_la_SOURCES = \
	WinMgr.h \
	applet.h \
	appnames.h \
	ascii.h \
	base.h \
	bindkey.cc \
	bindkey.h \
	debug.h \
	default.h \
	globit.cc \
	globit.h \
	intl.h \
	logevent.h \
	mstring.h \
	prefs.h \
	ref.h \
	sysdep.h \
	themable.h \
	udir.h \
	upath.h \
	wmaction.h \
	wmclient.h \
	wmconfig.cc \
	wmconfig.h \
	wmmgr.h \
	wmoption.h \
	wmsave.cc \
	wmsave.h \
	wmtaskbar.h \
	workspaces.h \
	wpixmaps.h \
	wpixres.cc \
	wpixres.h \
	yaction.h \
	yapp.h \
	yarray.h \
	ybutton.cc \
	ybutton.h \
	ycolor.h \
	yconfig.h \
	ycursor.h \
	yfull.h \
	yicon.cc \
	yicon.h \
	yimage.h \
	yinputline.cc \
	yinputline.h \
	ykey.h \
	ylabel.cc \
	ylabel.h \
	ylayout.h \
	ylib.h \
	ylist.h \
	ylistbox.cc \
	ylistbox.h \
	ymenu.cc \
	ymenu.h \
	ymenuitem.cc \
	ymenuitem.h \
	ymsgbox.h \
	ypaint.h \
	ypixmap.h \
	ypointer.h \
	ypoll.h \
	ypopup.h \
	yprefs.h \
	yrect.h \
	yscrollbar.cc \
	yscrollbar.h \
	yscrollview.cc \
	yscrollview.h \
	ytime.h \
	ytimer.h \
	ytrace.h \
	ywindow.h \
	ywordexp.h \
	yxapp.h \
	yxtray.h
libitk_la_LIBADD = libice.la

genpref_SOURCES = \
	base.h \
	ascii.h \
	appnames.h \
	ykey.h \
	sysdep.h \
	yprefs.h \
	bindkey.h \
	default.h \
	themable.h \
	icewmbg_prefs.h \
	genpref.cc
genpref_LDADD = libice.la @LIBINTL@ @LIBICONV@

icewm_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	bindkey.h \
	themable.h \
	default.h \
	prefs.h \
	obj.h \
	yaction.h \
	yinputline.h \
	yactionbutton.h \
	objmenu.h \
	objbutton.h \
	objbutton.cc \
	WinMgr.h \
	ypoll.h \
	ylib.h \
	ymsgbox.cc \
	ymsgbox.h \
	ydialog.cc \
	ydialog.h \
	yurl.cc \
	yurl.h \
	yrect.h \
	wmsession.cc \
	wmsession.h \
	wmwinlist.cc \
	wmwinlist.h \
	wmtaskbar.cc \
	wmtaskbar.h \
	wmwinmenu.cc \
	wmwinmenu.h \
	wmdialog.cc \
	wmdialog.h \
	wmabout.cc \
	wmabout.h \
	wmswitch.cc \
	wmswitch.h \
	wmstatus.cc \
	wmstatus.h \
	wmoption.cc \
	wmoption.h \
	wmaction.h \
	wmcontainer.cc \
	wmcontainer.h \
	wmclient.cc \
	MwmUtil.h \
	wmclient.h \
	wmdock.cc \
	wmdock.h \
	wmmgr.cc \
	wmmgr.h \
	workspaces.h \
	appnames.h \
	guievent.h \
	wmapp.cc \
	wmapp.h \
	wmframe.cc \
	wmframe.h \
	wmbutton.cc \
	wmbutton.h \
	wmminiicon.cc \
	wmminiicon.h \
	wmtitle.cc \
	wmtitle.h \
	movesize.cc \
	ascii.h \
	themes.cc \
	themes.h \
	theminst.cc \
	theminst.h \
	decorate.cc \
	browse.cc \
	browse.h \
	wmmenu.cc \
	wmprog.cc \
	wmprog.h \
	wmpref.cc \
	wmpref.h \
	atasks.cc \
	atasks.h \
	aworkspaces.cc \
	aworkspaces.h \
	amailbox.cc \
	amailbox.h \
	akeyboard.cc \
	akeyboard.h \
	aclock.cc \
	aclock.h \
	acpustatus.cc \
	acpustatus.h \
	amemstatus.cc \
	amemstatus.h \
	applet.cc \
	applet.h \
	apppstatus.cc \
	apppstatus.h \
	aaddressbar.cc \
	aaddressbar.h \
	objbar.cc \
	objbar.h \
	aapm.cc \
	aapm.h \
	atray.cc \
	atray.h \
	ysmapp.cc \
	ysmapp.h \
	yxcontext.h \
	yxtray.cc \
	yxtray.h
icewm_LDADD = libitk.la libice.la $(IMAGE_LIBS) $(XSM_LIBS) $(CORE_LIBS) @LIBINTL@ @LIBICONV@

icesh_SOURCES = \
	intl.h \
	ascii.h \
	debug.h \
	base.h \
	MwmUtil.h \
	WinMgr.h \
	guievent.h \
	logevent.h \
	wmaction.h \
	ypointer.h \
	ytime.h \
	yrect.h \
	icesh.cc
icesh_LDADD = libice.la $(X_LIBS) $(RANDR_LIBS) $(XINERAMA_LIBS) $(XRES_LIBS) -lX11 @LIBINTL@ @LIBICONV@

icewm_session_SOURCES = \
	appnames.h \
	base.h \
	debug.h \
	intl.h \
	sysdep.h \
	yapp.h \
	ytime.h \
	ytimer.h \
	icesm.cc
icewm_session_LDADD = libice.la $(X_LIBS) -lX11 @LIBINTL@ @LIBICONV@

icewmhint_SOURCES = \
	intl.h \
	debug.h \
	base.h \
	icewmhint.cc
icewmhint_LDADD = libice.la $(CORE_LIBS) @LIBINTL@ @LIBICONV@

icewmbg_SOURCES = \
	intl.h \
	debug.h \
	base.h \
	themable.h \
	default.h \
	yxapp.h \
	yprefs.h \
	icewmbg.cc \
	icewmbg_prefs.h
icewmbg_LDADD = libice.la $(IMAGE_LIBS) $(CORE_LIBS) @LIBINTL@ @LIBICONV@

icesound_SOURCES = \
	base.h \
	debug.h \
	guievent.h \
	intl.h \
	upath.h \
	ypointer.h \
	ytime.h \
	ytimer.h \
	icesound.cc
icesound_LDADD = libice.la $(AUDIO_LIBS) $(CORE_LIBS) @LIBINTL@ @LIBICONV@

icewm_menu_fdo_SOURCES = \
	appnames.h \
	base.h \
	debug.h \
	fdospecgen.h \
	intl.h \
	sysdep.h \
	yarray.h \
	sysdep.h \
	fdomenu.cc
icewm_menu_fdo_LDADD = libice.la $(GIO_LIBS)

icehelp_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	ascii.h \
	bindkey.h \
	themable.h \
	default.h \
	prefs.h \
	yaction.h \
	ylib.h \
	yrect.h \
	icehelp.cc
icehelp_LDADD = libitk.la libice.la $(IMAGE_LIBS) $(CORE_LIBS) @LIBINTL@ @LIBICONV@

iceclock_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	iceclock.cc \
	bindkey.h \
	themable.h \
	default.h \
	prefs.h \
	applet.cc \
	applet.h \
	aclock.cc \
	aclock.h
iceclock_LDADD = libitk.la libice.la $(IMAGE_LIBS) $(CORE_LIBS) @LIBINTL@ @LIBICONV@

strtest_SOURCES = \
	base.h \
	udir.h \
	upath.h \
	strtest.cc \
	mstring.h
strtest_LDADD = libice.la

icewmtray_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	bindkey.h \
	themable.h \
	default.h \
	prefs.h \
	yxtray.cc \
	yxtray.h \
	wmoption.cc \
	wmoption.h \
	ylib.h \
	icetray.cc
icewmtray_LDADD = libitk.la libice.la $(IMAGE_LIBS) $(CORE_LIBS)

icesame_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	MwmUtil.h \
	yaction.h \
	yrect.h \
	ykey.h \
	yfull.h \
	icesame.cc
icesame_LDADD = libitk.la libice.la $(IMAGE_LIBS) $(CORE_LIBS)

icelist_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	yaction.h \
	yinputline.h \
	yrect.h \
	ykey.h \
	yfull.h \
	icelist.cc
icelist_LDADD = libitk.la libice.la $(IMAGE_LIBS) $(CORE_LIBS)

iceview_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	bindkey.h \
	themable.h \
	default.h \
	prefs.h \
	yaction.h \
	ylib.h \
	yrect.h \
	iceview.cc
iceview_LDADD = libitk.la libice.la $(IMAGE_LIBS) $(CORE_LIBS)

iceicon_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	yaction.h \
	ylib.h \
	yrect.h \
	iceicon.cc
iceicon_LDADD = libitk.la libice.la $(IMAGE_LIBS) $(CORE_LIBS)

icerun_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	bindkey.h \
	themable.h \
	default.h \
	prefs.h \
	yaction.h \
	yinputline.h \
	icerun.cc
icerun_LDADD = libitk.la libice.la $(IMAGE_LIBS) $(CORE_LIBS)

iceskt_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	iceskt.cc
iceskt_LDADD = libitk.la libice.la $(IMAGE_LIBS) $(CORE_LIBS)

testmenus_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	obj.h \
	yaction.h \
	testmenus.cc \
	bindkey.h \
	themable.h \
	default.h \
	prefs.h \
	wmmenu.cc \
	wmprog.cc \
	wmprog.h \
	wmaction.h \
	ascii.h \
	themes.cc \
	themes.h \
	browse.cc \
	browse.h
testmenus_LDADD = libitk.la libice.la $(IMAGE_LIBS) $(CORE_LIBS)

testwinhints_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	WinMgr.h \
	testwinhints.cc
testwinhints_LDFLAGS = $(CORE_LIBS)

testnetwmhints_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	WinMgr.h \
	testnetwmhints.cc
testnetwmhints_LDFLAGS = libice.la $(CORE_LIBS)

testmap_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	WinMgr.h \
	testmap.cc
testmap_LDFLAGS = libice.la $(CORE_LIBS)

testlocale_SOURCES = \
	intl.h \
	debug.h \
	sysdep.h \
	base.h \
	ylib.h \
	testlocale.cc
testlocale_LDADD = libice.la $(CORE_LIBS) @LIBINTL@ @LIBICONV@

testarray_SOURCES = \
	mstring.h \
	yarray.h \
	base.h \
	ypointer.h \
	testarray.cc
testarray_LDADD = libice.la @LIBINTL@ @LIBICONV@

testpointer_SOURCES = \
	ypointer.h \
	testpointer.cc

nodist_pkgdata_DATA = \
	preferences

preferences: genpref$(EXEEXT)
	$(AM_V_GEN)./genpref$(EXEEXT) -o $@ -s

CLEANFILES = preferences strtest testarray testpointer

