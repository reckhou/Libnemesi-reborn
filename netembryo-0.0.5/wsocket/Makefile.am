
netembryoincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libwsocket.la

libwsocket_la_LDFLAGS = 

libwsocket_la_SOURCES =		is_multicast_address.c \
				gethostinfo.c \
				mcast_join.c \
				mcast_leave.c \
				sock_accept.c \
				sock_bind.c \
				sock_close.c \
				sock_connect.c \
				sock_get_port.c \
				sock_listen.c \
				sock_ntop_host.c \
				sockfd_to_family.c \
				get_info.c \
				Sock_accept.c \
				Sock_bind.c \
				Sock_close.c \
				Sock_compare.c \
				Sock_connect.c \
				Sock_init.c \
				Sock_listen.c \
				Sock_set_props.c \
				Sock_read.c \
				Sock_write.c \
				Sock_create_ssl_connection.c \
				Sock_socketpair.c \
				Sock_set_dest.c

if HAVE_SSL
libwsocket_la_SOURCES +=	create_ssl_ctx.c \
				get_ssl_connection.c \
				sock_SSL_accept.c \
				sock_SSL_close.c \
				sock_SSL_connect.c \
				sock_SSL_read.c \
				sock_SSL_write.c

endif

#if HAVE_SSL 
#libwsocket_la_LDFLAGS += $(SSL_LDFLAGS)
#libwsocket_la_LIBADD  = $(SSL_LIBS)
#endif

INCLUDES = -I$(netembryoincludedir) -I$(top_srcdir)

#if HAVE_SSL
#INCLUDES += $(SSL_CFLAG)
#endif
