/* * 
 * This file is part of NetEmbryo
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * NetEmbryo is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * NetEmbryo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NetEmbryo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA 
 *
 * */


#ifndef WSOCKET_H
#define WSOCKET_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#ifndef WIN32
#   include <unistd.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#   include <netdb.h>
#else
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   include <stdint.h>
#endif


#ifdef HAVE_LIBSCTP
#include <netinet/sctp.h>
#define MAX_SCTP_STREAMS 15
#endif

#if HAVE_SSL
#include <openssl/ssl.h>
#endif

#ifndef IPV6_ADD_MEMBERSHIP
#define IPV6_ADD_MEMBERSHIP IPV6_JOIN_GROUP
#define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP
#endif


#ifndef IN_IS_ADDR_MULTICAST
#define IN_IS_ADDR_MULTICAST(a)    ((((in_addr_t)(a)) & 0xf0000000) == 0xe0000000)
#endif

#if IPV6
#ifndef IN6_IS_ADDR_MULTICAST
#define IN6_IS_ADDR_MULTICAST(a) ((a)->s6_addr[0] == 0xff)
#endif
#endif //IPV6

#ifdef WORDS_BIGENDIAN
#define ntohl24(x) (x)
#else
#define ntohl24(x) (((x&0xff) << 16) | (x&0xff00) | ((x&0xff0000)>>16)) 
#endif

#ifdef WIN32
typedef unsigned short sa_family_t;
typedef unsigned short in_port_t;
typedef unsigned int in_addr_t;
#endif

#ifndef HAVE_STRUCT_SOCKADDR_STORAGE
/* Structure large enough to hold any socket address (with the historical exception of 
AF_UNIX). 128 bytes reserved.  */
#if ULONG_MAX > 0xffffffff
# define __ss_aligntype __uint64_t
#else
# define __ss_aligntype __uint32_t
#endif
#define _SS_SIZE        128
#define _SS_PADSIZE     (_SS_SIZE - (2 * sizeof (__ss_aligntype)))

struct sockaddr_storage
{
    sa_family_t ss_family;      /* Address family */
    __ss_aligntype __ss_align;  /* Force desired alignment.  */
    char __ss_padding[_SS_PADSIZE];
};
#endif // HAVE_STRUCT_SOCKADDR_STORAGE

/** flags definition*/
typedef enum {
/** ssl flags */
    IS_SSL = 0x1,
    IS_TLS = 0x3, /**< setting this will also set IS_SSL */
/** multicast flags */
    IS_MULTICAST = 0x4
} sock_flags;

/** socket type definition */
typedef enum {
/** socket fd not valid */
    SOCK_NONE,
/** IP based protcols */
    TCP,
    UDP,
    SCTP,
/** Local socket (Unix) */
    LOCAL
} sock_type;

/* NOTE:
 *    struct ip_mreq {
 *        struct in_addr imr_multiaddr;
 *        struct in_addr imr_interface;
 *    }
 *
 *    struct ipv6_mreq {
 *        struct in6_addr    ipv6mr_multiaddr;
 *        unsigned int ipv6mr_interface;
 *    }
 */

#if IPV6
/** multicast IPv6 storage structure */
struct ipv6_mreq_in6 {
    struct ipv6_mreq NETmreq6;
    struct in6_addr __imr_interface6;
};
#endif
/** multicast IPv4 storage structure */
struct ip_mreq_in {
    struct ip_mreq NETmreq;
    unsigned int __ipv4mr_interface;
};

#if 0
union ADDR {
    struct in_addr in;
    struct in6_addr in6;
};
#endif

union ADDR {
#if IPV6
    struct ipv6_mreq_in6 mreq_in6; /*struct in6_addr ipv6mr_multiaddr; struct in6_addr imr_interface6 ; unsigned int ipv6mr_interface; */
#endif //IPV6
    struct ip_mreq_in mreq_in; /*struct in_addr ipv4mr_multiaddr; struct in_addr imr_interface4; unsigned int ipv4mr_interface;*/
};
#if IPV6
    #define imr_interface6 __imr_interface6
    #define ipv6_interface NETmreq6.ipv6mr_interface
    #define ipv6_multiaddr NETmreq6.ipv6mr_multiaddr
#endif //IPV6
    #define ipv4_interface __ipv4mr_interface
    #define imr_interface4 NETmreq.imr_interface
    #define ipv4_multiaddr NETmreq.imr_multiaddr

/* Developer HowTo: //TODO: Update to new multicast API
 *
 * union ADDR
 *         struct ipv6_mreq_in6 mreq_in6
 *             struct in6_addr ipv6_multiaddr    // IPv6 class D multicast address. defined =  NETmreq6.ipv6mr_multiaddr
 *             struct in6_addr imr_interface6    // IPv6 address of local interface.
 *             unsigned int ipv6_interface    // interface index, or 0
 *             struct ipv6_mreq NETmreq6
 *          struct ip_mreq_in mreq_in
 *              struct in_addr ipv4_multiaddr     // IPv4 class D multicast address. defined = NETmreq.imr_multiaddr
 *              struct in_addr imr_interface4    // IPv4 address of local interface. defined = NETmreq.imr_interface
 *              unsigned int ipv4_interface    // interface index, or 0
 *              struct ip_mreq NETmreq
 */

/** socket storage structure */
typedef struct {
    int fd;    ///< low level socket file descriptor
    struct sockaddr_storage local_stg;    ///< low level address storage from getsockname
    struct sockaddr_storage remote_stg;    ///< low level address storage from getpeername
    sock_type socktype; ///< socket type enumeration
    union ADDR addr; ///< multicast address storage
    /** flags */
    sock_flags flags;
    /** human readable datas */
    char *remote_host; ///< remote host stored as dinamic string
    char *local_host; ///< local host stored as dinamic string
    in_port_t remote_port;    ///< remote port stored in host order
    in_port_t local_port;    ///< local port stored in host order
#if HAVE_SSL
    SSL *ssl; ///< stores ssl context information
#endif
} Sock;

#define WSOCK_ERRORPROTONOSUPPORT -5    
#define WSOCK_ERRORIOCTL    -4    
#define WSOCK_ERRORINTERFACE    -3    
#define WSOCK_ERROR    -2    
#define WSOCK_ERRFAMILYUNKNOWN    -1
#define WSOCK_OK 0
#define WSOCK_ERRSIZE    1
#define WSOCK_ERRFAMILY    2
#define WSOCK_ERRADDR    3
#define WSOCK_ERRPORT    4

/** low level wrappers */
int sockfd_to_family(int sockfd);
int gethostinfo(struct addrinfo **res, char *host, char *serv, struct addrinfo *hints); //TODO: Remove
int sock_connect(char *host, char *port, int *sock, sock_type socktype);
int sock_bind(char *host, char *port, int *sock, sock_type socktype);
int sock_accept(int sock);
int sock_listen(int s, int backlog);
int sock_close(int s);

/** host & port wrappers */
/* return the address in human readable string format */
const char *sock_ntop_host(const struct sockaddr *sa, char *str, size_t len);
/* return the port in network byte order (use ntohs to change it) */
int32_t sock_get_port(const struct sockaddr *sa);

/** multicast*/
int16_t is_multicast_address(const struct sockaddr *sa, sa_family_t family);
int mcast_join (int sockfd, const struct sockaddr *sa/*, socklen_t salen*/, const char *ifname, unsigned int ifindex, union ADDR *addr);
int mcast_leave(int sockfd, const struct sockaddr *sa/*, socklen_t salen*/);

#if HAVE_SSL
/** ssl wrappers */
SSL_CTX *create_ssl_ctx(void);
SSL *get_ssl_connection(int);
int sock_SSL_connect(SSL **, int);
int sock_SSL_accept(SSL **, int);
int sock_SSL_read(SSL *, void *, int);
int sock_SSL_write(SSL *, void *, int);
int sock_SSL_close(SSL *);
#endif

/** log facilities */
/* Outputs the messages using the default logger o a custom one passed to
 * Sock_init() */
void net_log(int, const char*, ...);
/* levels to be implemented by log function */
#define NET_LOG_FATAL 0 
#define NET_LOG_ERR 1
#define NET_LOG_WARN 2 
#define NET_LOG_INFO 3 
#define NET_LOG_DEBUG 4 
#define NET_LOG_VERBOSE 5 

/** @defgroup NetEmbryo_Socket Sockets Access Interface
 *
 * @brief These functions offer high level network connectivity for IP and Unix protocols
 *
 * @{ */

/** Establish a connection to a remote host.
 *  @param host Remote host to connect to (may be a hostname).
 *  @param port Remote port to connect to.
 *  @param binded Pointer to a pre-binded socket (useful for connect from a specific interface/port),
 *  if NULL a new socket will be created.
 *  @param socktype The type of socket to be created.
 *  @param ssl_flag Enables ssl and/or multicast.
 */
Sock * Sock_connect(char *host, char *port, Sock *binded, sock_type socktype, sock_flags ssl_flag);
/** Create a new socket and binds it to an address/port.
 *  @param host Local address to be used by this socket, if NULL the socket will
 *  be bound to all interfaces.
 *  @param port Local port to be used by this socket, if NULL a random port will
 *  be used.
 *  @param socktype The type of socket to be created.
 *  @param ssl_flag Enables ssl and/or multicast.
 */
Sock * Sock_bind(char *host, char *port, sock_type socktype, sock_flags ssl_flag);
/** Create a new socket accepting a new connection from a listening socket.
 *  @param main Listening socket.
 */
Sock * Sock_accept(Sock *main);
/** Setup ssl on an existing connected socket.
 *  @param s Existing socket.
 */
int Sock_create_ssl_connection(Sock *s);
/** Put a socket in listening state.
 *  @param s Existing socket.
 *  @param backlog Number of connection that may wait to be accepted.
 */
int Sock_listen(Sock *s, int backlog);
/** Read data from a socket.
 *  @param s Existing socket.
 *  @param buffer Buffer reserved for receiving data.
 *  @param nbytes Size of the buffer.
 *  @param protodata Pointer to data depending from socket protocol, if NULL a
 *  suitable default value will be used.
 *  @param flags Flags to be passed to posix recv() function.
 */
int Sock_read(Sock *s, void *buffer, int nbytes, void *protodata, int flags);
/** Read data to a socket
 *  @param s Existing socket.
 *  @param buffer Buffer of data to be sent.
 *  @param nbytes Amount of data to be sent.
 *  @param protodata Pointer to data depending from socket protocol, if NULL a
 *  suitable default value will be used.
 *  @param flags Flags to be passed to posix send() function.
 */
int Sock_write(Sock *s, void *buffer, int nbytes, void *protodata, int flags);
/** Close an existing socket.
 *  @param s Existing socket.
 */
int Sock_close(Sock *s);
/** Inits internal structures.
 *  @param log_function Pointer to a proper log function, if NULL messages will
 *  be sent to stderr.
 */
void Sock_init(void (*log_function)(int level, const char *fmt, va_list list));
/** Compare two sockets.
 *  @param p Existing socket.
 *  @param q Existing socket.
 */
int Sock_compare(Sock *p, Sock *q);
#define Sock_cmp Sock_compare
/** Creates and connect together two sockets.
 *  @param pair A vector large enough for two socket structures.
 */
int Sock_socketpair(Sock *pair[]);
/** Change destination address for a non connected protocol socket (like UDP).
 *  @param s Existing non connected socket.
 *  @param dst Destination address.
 */
int Sock_set_dest(Sock *s, struct sockaddr *dst);

/** low level access macros */
#define Sock_fd(A) ((A)->fd)
#define Sock_type(A) ((A)->socktype)
#define Sock_flags(A) ((A)->flags)

/** Set ioctl properties for socket
 *  @return Usually, on success zero. A few ioctls use the return value as an
 *  output parameter and return a nonnegative value on success. On error, -1 is
 *  returned, and errno is set appropriately.
 */
int Sock_set_props(Sock *s, int request, int *on);

/*get_info.c*/
char * get_remote_host(Sock *);
char * get_local_host(Sock *);
inline int get_local_hostname(char *localhostname, size_t len); // return 0 if ok
in_port_t get_remote_port(Sock *);
in_port_t get_local_port(Sock *);

/**
 * @}
 */
 
#endif // WSOCKET_H
