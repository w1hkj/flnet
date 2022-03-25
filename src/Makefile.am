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
	include/compat.h compat/getsysinfo.c compat/mingw.c compat/mingw.h \
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

# Build the libmbedtls source if libmbedtls is not found
LIBMBEDTLS_SRC = \
	mbedtls/aes.c \
	mbedtls/aesni.c \
	mbedtls/arc4.c \
	mbedtls/aria.c \
	mbedtls/asn1parse.c \
	mbedtls/asn1write.c \
	mbedtls/base64.c \
	mbedtls/bignum.c \
	mbedtls/blowfish.c \
	mbedtls/camellia.c \
	mbedtls/ccm.c \
	mbedtls/certs.c \
	mbedtls/chacha20.c \
	mbedtls/chachapoly.c \
	mbedtls/cipher.c \
	mbedtls/cipher_wrap.c \
	mbedtls/cmac.c \
	mbedtls/ctr_drbg.c \
	mbedtls/debug.c \
	mbedtls/des.c \
	mbedtls/dhm.c \
	mbedtls/ecdh.c \
	mbedtls/ecdsa.c \
	mbedtls/ecjpake.c \
	mbedtls/ecp.c \
	mbedtls/ecp_curves.c \
	mbedtls/entropy.c \
	mbedtls/entropy_poll.c \
	mbedtls/error.c \
	mbedtls/gcm.c \
	mbedtls/havege.c \
	mbedtls/hkdf.c \
	mbedtls/hmac_drbg.c \
	mbedtls/md2.c \
	mbedtls/md4.c \
	mbedtls/md5.c \
	mbedtls/md.c \
	mbedtls/md_wrap.c \
	mbedtls/memory_buffer_alloc.c \
	mbedtls/net_sockets.c \
	mbedtls/nist_kw.c \
	mbedtls/oid.c \
	mbedtls/padlock.c \
	mbedtls/pem.c \
	mbedtls/pk.c \
	mbedtls/pkcs11.c \
	mbedtls/pkcs12.c \
	mbedtls/pkcs5.c \
	mbedtls/pkparse.c \
	mbedtls/pk_wrap.c \
	mbedtls/pkwrite.c \
	mbedtls/platform.c \
	mbedtls/platform_util.c \
	mbedtls/poly1305.c \
	mbedtls/ripemd160.c \
	mbedtls/rsa.c \
	mbedtls/rsa_internal.c \
	mbedtls/sha1.c \
	mbedtls/sha256.c \
	mbedtls/sha512.c \
	mbedtls/ssl_cache.c \
	mbedtls/ssl_ciphersuites.c \
	mbedtls/ssl_cli.c \
	mbedtls/ssl_cookie.c \
	mbedtls/ssl_srv.c \
	mbedtls/ssl_ticket.c \
	mbedtls/ssl_tls.c \
	mbedtls/threading.c \
	mbedtls/timing.c \
	mbedtls/version.c \
	mbedtls/version_features.c \
	mbedtls/x509.c \
	mbedtls/x509_create.c \
	mbedtls/x509_crl.c \
	mbedtls/x509_crt.c \
	mbedtls/x509_csr.c \
	mbedtls/x509write_crt.c \
	mbedtls/x509write_csr.c \
	mbedtls/xtea.c \
	include/mbedtls/aes.h \
	include/mbedtls/aesni.h \
	include/mbedtls/arc4.h \
	include/mbedtls/aria.h \
	include/mbedtls/asn1.h \
	include/mbedtls/asn1write.h \
	include/mbedtls/base64.h \
	include/mbedtls/bignum.h \
	include/mbedtls/blowfish.h \
	include/mbedtls/bn_mul.h \
	include/mbedtls/camellia.h \
	include/mbedtls/ccm.h \
	include/mbedtls/certs.h \
	include/mbedtls/chacha20.h \
	include/mbedtls/chachapoly.h \
	include/mbedtls/check_config.h \
	include/mbedtls/cipher.h \
	include/mbedtls/cipher_internal.h \
	include/mbedtls/cmac.h \
	include/mbedtls/compat-1.3.h \
	include/mbedtls/config.h \
	include/mbedtls/ctr_drbg.h \
	include/mbedtls/debug.h \
	include/mbedtls/des.h \
	include/mbedtls/dhm.h \
	include/mbedtls/ecdh.h \
	include/mbedtls/ecdsa.h \
	include/mbedtls/ecjpake.h \
	include/mbedtls/ecp.h \
	include/mbedtls/ecp_internal.h \
	include/mbedtls/entropy.h \
	include/mbedtls/entropy_poll.h \
	include/mbedtls/error.h \
	include/mbedtls/gcm.h \
	include/mbedtls/havege.h \
	include/mbedtls/hkdf.h \
	include/mbedtls/hmac_drbg.h \
	include/mbedtls/md2.h \
	include/mbedtls/md4.h \
	include/mbedtls/md5.h \
	include/mbedtls/md.h \
	include/mbedtls/md_internal.h \
	include/mbedtls/memory_buffer_alloc.h \
	include/mbedtls/net.h \
	include/mbedtls/net_sockets.h \
	include/mbedtls/nist_kw.h \
	include/mbedtls/oid.h \
	include/mbedtls/padlock.h \
	include/mbedtls/pem.h \
	include/mbedtls/pkcs11.h \
	include/mbedtls/pkcs12.h \
	include/mbedtls/pkcs5.h \
	include/mbedtls/pk.h \
	include/mbedtls/pk_internal.h \
	include/mbedtls/platform.h \
	include/mbedtls/platform_time.h \
	include/mbedtls/platform_util.h \
	include/mbedtls/poly1305.h \
	include/mbedtls/ripemd160.h \
	include/mbedtls/rsa.h \
	include/mbedtls/rsa_internal.h \
	include/mbedtls/sha1.h \
	include/mbedtls/sha256.h \
	include/mbedtls/sha512.h \
	include/mbedtls/ssl_cache.h \
	include/mbedtls/ssl_ciphersuites.h \
	include/mbedtls/ssl_cookie.h \
	include/mbedtls/ssl.h \
	include/mbedtls/ssl_internal.h \
	include/mbedtls/ssl_ticket.h \
	include/mbedtls/threading.h \
	include/mbedtls/timing.h \
	include/mbedtls/version.h \
	include/mbedtls/x509_crl.h \
	include/mbedtls/x509_crt.h \
	include/mbedtls/x509_csr.h \
	include/mbedtls/x509.h \
	include/mbedtls/xtea.h

# We distribute these but do not always compile them
EXTRA_flnet_SOURCES = \
	$(FLMSG_WIN32_RES_SRC) $(MINGW32_SRC) \
	$(XMLRPCPP_SRC) $(LIBMBEDTLS_SRC)

flnet_SOURCES =

if !ENABLE_FLXMLRPC
flnet_SOURCES += $(XMLRPCPP_SRC)
else
flnet_CPPFLAGS += @FLXMLRPC_CFLAGS@
flnet_CXXFLAGS += @FLXMLRPC_CFLAGS@
flnet_CFLAGS   += @FLXMLRPC_CFLAGS@
flnet_LDFLAGS  += @FLXMLRPC_LIBS@
endif

if !ENABLE_LIBMBEDTLS
flnet_SOURCES += $(LIBMBEDTLS_SRC)
else
flnet_CPPFLAGS += @LIBMBEDTLS_CFLAGS@
flnet_CXXFLAGS += @LIBMBEDTLS_CFLAGS@
flnet_CFLAGS   += @LIBMBEDTLS_CFLAGS@
flnet_LDFLAGS  += @LIBMBEDTLS_LIBS@
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
	network/ca_cert.cxx \
	network/network.cxx \
	network/socket.cxx \
	support/calendar.cxx \
	support/combo.cxx \
	support/config.cxx \
	csv_dbase/csvdb.cxx \
	support/date.cxx \
	support/debug.cxx \
	support/font_browser.cxx \
	support/icons.cxx \
	support/loglist.cxx \
	support/locator.cxx \
	support/lookupcall.cxx \
	support/masterdb.cxx \
	support/netshared.cxx \
	support/netsupport.cxx \
	support/pixmaps.cxx \
	support/pixmaps_tango.cxx \
	support/status.cxx \
	support/threads.cxx \
	support/timeops.cxx \
	support/util.cxx \
	support/xml_io.cxx \
	UI/flinput2.cxx \
	UI/my_UI.cxx \
	UI/netedits.cxx \
	UI/net_ui.cxx \
	UI/netutils.cxx \
	UI/sorting.cxx \
	UI/ui_sizes.cxx \
	include/calendar.h \
	include/combo.h \
	include/csvdb.h \
	include/estrings.h \
	include/lookupcall.h \
	include/net_config.h \
	include/date.h \
	include/debug.h \
	include/gettext.h \
	include/icons.h \
	include/loglist.h \
	include/my_UI.h \
	include/netedits.h \
	include/net_icon.h \
	include/flinput2.h \
	include/flnetrc.h \
	include/font_browser.h \
	include/locator.h \
	include/masterdb.h \
	include/netshared.h \
	include/netsupport.h \
	include/net_ui.h \
	include/netutils.h \
	include/netversion.h \
	include/network.h \
	include/pixmaps.h \
	include/socket.h \
	include/sorting.h \
	include/status.h \
	include/timeops.h \
	include/util.h \
	include/xmlreader.h \
	include/xmlrpc.h \
	include/xml_io.h \
	include/CXMLReaderImpl.h \
	include/fast_atof.h \
	include/heapsort.h \
	include/irrArray.h \
	include/irrString.h \
	include/irrTypes.h \
	include/irrXML.h \
	irrxml/irrXML.cpp

# Additional source files that are distributed
EXTRA_DIST = \
	compat/mingw.c \
	compat/mingw.h \
	compat/getsysinfo.c \
	compat/regex.c \
	compat/regex.h \
	icons/calendar.xpm \
	include/threads.h \
	include/fl_lock.h \
	UI/flnet.rc \
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
	$(srcdir)/mbedtls/libmbedtls.pc \
	$(srcdir)/mbedtls/LICENSE \
	$(srcdir)/mbedtls/gpl-2.0.txt \
	$(flnet_WIN32_SRC) \
	$(flnet_FL_SRC)
