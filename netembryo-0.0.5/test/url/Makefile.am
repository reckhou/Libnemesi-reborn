netembryoincludedir = $(top_srcdir)/include

bin_PROGRAMS = url_test

url_test_SOURCES =	main.c		 
			
			
url_test_LDADD =	$(top_srcdir)/wsocket/libwsocket.la
url_test_LDADD +=  $(top_srcdir)/utils/libutils.la

INCLUDES = -I$(netembryoincludedir) -I$(top_srcdir)
