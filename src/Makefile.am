# Copyright (c) 2008 Dave Freese, W1HKJ (w1hkj AT w1hkj DOT com)

bin_PROGRAMS = flnet

flnet_CPPFLAGS = @FLNET_BUILD_CPPFLAGS@
flnet_CXXFLAGS = @FLNET_BUILD_CXXFLAGS@
flnet_CFLAGS = $(flnet_CXXFLAGS)
flnet_LDFLAGS = @FLNET_BUILD_LDFLAGS@
flnet_LDADD = @FLNET_BUILD_LDADD@

MINGW32_SRC = \
	UI/flnet.rc \
	include/flnetrc.h \
	support/mingw.c \
	include/compat.h \
	include/compat-mingw.h

# Build the xmlrpcpp source if libflxmlrpc is not found
XMLRPCPP_SRC = \
	xmlrpcpp/XmlRpcBase64.h \
	xmlrpcpp/XmlRpcClient.cpp \
	xmlrpcpp/XmlRpcClient.h \
	xmlrpcpp/XmlRpcDispatch.cpp \
	xmlrpcpp/XmlRpcDispatch.h \
	xmlrpcpp/XmlRpcException.h \
	xmlrpcpp/XmlRpc.h \
	xmlrpcpp/XmlRpcMutex.cpp \
	xmlrpcpp/XmlRpcMutex.h \
	xmlrpcpp/XmlRpcServerConnection.cpp \
	xmlrpcpp/XmlRpcServerConnection.h \
	xmlrpcpp/XmlRpcServer.cpp \
	xmlrpcpp/XmlRpcServer.h \
	xmlrpcpp/XmlRpcServerMethod.cpp \
	xmlrpcpp/XmlRpcServerMethod.h \
	xmlrpcpp/XmlRpcSocket.cpp \
	xmlrpcpp/XmlRpcSocket.h \
	xmlrpcpp/XmlRpcSource.cpp \
	xmlrpcpp/XmlRpcSource.h \
	xmlrpcpp/XmlRpcUtil.cpp \
	xmlrpcpp/XmlRpcUtil.h \
	xmlrpcpp/XmlRpcValue.cpp \
	xmlrpcpp/XmlRpcValue.h

# We distribute these but do not always compile them
EXTRA_flnet_SOURCES = $(FLMSG_WIN32_RES_SRC) $(MINGW32_SRC) $(XMLRPCPP_SRC)

flnet_SOURCES =

if !ENABLE_FLXMLRPC
flnet_SOURCES += $(XMLRPCPP_SRC)
else
flnet_CPPFLAGS += @FLXMLRPC_CFLAGS@
flnet_CXXFLAGS += @FLXMLRPC_CFLAGS@
flnet_CFLAGS   += @FLXMLRPC_CFLAGS@
flnet_LDFLAGS  += @FLXMLRPC_LIBS@
endif

if MINGW32
if HAVE_WINDRES
  flnet_SOURCES += $(MINGW32_SRC)
endif
endif

########################################################################


flnet_VERSION_MAJOR = @FLNET_VERSION_MAJOR@
flnet_VERSION_MINOR = @FLNET_VERSION_MINOR@
flnet_VERSION_PATCH = @FLNET_VERSION_PATCH@
flnet_VERSION = @FLNET_VERSION@

.EXPORT_ALL_VARIABLES: nsisinst appbundle

# Sources that are generated,
BUILT_SOURCES =
# not distributed,
nodist_flnet_SOURCES = $(BUILT_SOURCES)
# and deleted by the clean targets
CLEANFILES = $(BUILT_SOURCES)
CLEAN_LOCAL =

if MINGW32
if HAVE_WINDRES
.rc.o:
	$(WINDRES) -I$(srcdir)/include -I$(srcdir)/../data/win32 $< -O coff $@
endif
endif

install-data-local:
	if test -f $(srcdir)/../data/flnet.xpm; then \
	    $(mkinstalldirs) $(DESTDIR)/$(datadir)/pixmaps; \
	    $(INSTALL_DATA) $(srcdir)/../data/flnet.xpm $(DESTDIR)/$(datadir)/pixmaps; \
	fi
	if test -f $(srcdir)/../data/flnet.desktop; then \
	    $(mkinstalldirs) $(DESTDIR)/$(datadir)/applications; \
	    $(INSTALL_DATA) $(srcdir)/../data/flnet.desktop $(DESTDIR)/$(datadir)/applications; \
	fi

uninstall-local:
	rm -f $(DESTDIR)/$(datadir)/pixmaps/flnet.xpm
	rm -f $(DESTDIR)/$(datadir)/applications/flnet.desktop

FL_SRC = UI/net_ui.fl UI/netutils.fl

if HAVE_FLUID
flgen: $(FL_SRC)
	(cd $(srcdir)/include; \
	for f in $(FL_SRC); do \
	    c=$${f%.fl}.cxx; \
		h=$${f%.fl}.h; \
		h=$${h##*/}; \
	    $(FLUID) -c -o ../$$c -h $$h ../$$f; \
	done)
endif


INSTALLER_FILE = flnet-$(flnet_VERSION)_setup.exe
APPBUNDLE=flnet-$(flnet_VERSION)
APPBUNDLE_NOLIBS=$(APPBUNDLE)-nolibs

if HAVE_NSIS
nsisinst: $(bin_PROGRAMS)
	sh $(srcdir)/../scripts/mknsisinst.sh "$(srcdir)/../data" .
    CLEANFILES += $(INSTALLER_FILE)
endif

if DARWIN
appbundle: $(bin_PROGRAMS)
	sh $(srcdir)/../scripts/mkappbundle.sh "$(srcdir)/../data" .
    CLEAN_LOCAL += $(APPBUNDLE_NOLIBS) $(APPBUNDLE) $(APPBUNDLE)*.dmg
endif

#if HAVE_ASCIIDOC
#$(builddir)/../doc/guide.html: $(builddir)/../doc/guide.txt
#	$(MAKE) -C $(builddir)/../doc $(AM_MAKEFLAGS) guide.html

#$(builddir)/dialogs/guide.cxx: $(builddir)/../doc/guide.html
#	mkdir -p $(builddir)/dialogs
#	sed 's/"/\\"/g; s/$$/\\n\\/g; 1 s/.*/const char* szBeginner = "&/; $$ s/.*/&n";/' $< > $@
#dialogs/htmlstrings.cxx: $(builddir)/dialogs/guide.cxx

#distclean-local:
#	@if test "$(builddir)" != "$(srcdir)"; then \
#	    rm -f $(builddir)/dialogs/guide.cxx; \
#        fi
#endif

clean-local:
	-rm -rf $(CLEAN_LOCAL)

# Sources that we build. It is OK to have headers here.
flnet_SOURCES += \
	main.cxx \
	support/calendar.cxx \
	support/combo.cxx \
	support/config.cxx \
	csv_dbase/csvdb.cxx \
	support/date.cxx \
	support/loglist.cxx \
	support/netshared.cxx \
	support/netsupport.cxx \
	support/qrzlib.cxx \
	support/timeops.cxx \
	support/xml_io.cxx \
	UI/my_UI.cxx \
	UI/netedits.cxx \
	UI/net_ui.cxx \
	UI/netutils.cxx \
	UI/ui_sizes.cxx \
	include/calendar.h \
	include/combo.h \
	include/csvdb.h \
	include/net_config.h \
	include/date.h \
	include/loglist.h \
	include/my_UI.h \
	include/netedits.h \
	include/net_icon.h \
	include/flnetrc.h \
	include/netshared.h \
	include/netsupport.h \
	include/net_ui.h \
	include/netutils.h \
	include/netversion.h \
	include/qrzlib.h \
	include/timeops.h \
	include/util.h \
	include/xmlreader.h \
	include/xmlrpc.h \
	include/xml_io.h

# Additional source files that are distributed
EXTRA_DIST = \
	icons/calendar.xpm \
	include/threads.h \
	include/fl_lock.h \
	UI/flnet.rc \
	UI/net_ui.fl \
	UI/big_ui.fl \
	UI/small_ui.fl \
	UI/netutils.fl \
	UI/net_icon.cxx

# Additional non-source files that are distributed
# Additional source files that support non-Linux cross compilation
EXTRA_DIST += \
	$(srcdir)/../data/flnet.desktop \
	$(srcdir)/../data/flnet.xpm \
	$(srcdir)/../data/win32/fl_app.nsi \
	$(srcdir)/../data/win32/flnet.ico \
	$(srcdir)/../scripts/mknsisinst.sh \
	$(srcdir)/../scripts/build4mingw.sh \
	$(srcdir)/../data/mac/Info.plist.in \
	$(srcdir)/../data/mac/flnet.icns \
	$(srcdir)/../scripts/mkappbundle.sh \
	$(flnet_WIN32_SRC) \
	$(flnet_FL_SRC)
