SUBDIRS =	wsocket \
		utils

EXTRA_DIST = 	config \
		include 

lib_LTLIBRARIES = libnetembryo.la

libincludedir = $(includedir)/netembryo
libinclude_HEADERS =    include/wsocket.h \
			include/url.h \
			include/rtsp_errors.h

libnetembryo_la_SOURCES =

libnetembryo_la_LIBADD =   wsocket/libwsocket.la
libnetembryo_la_LIBADD +=   utils/libutils.la

#pkg-config
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libnetembryo.pc

libnetembryo_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
