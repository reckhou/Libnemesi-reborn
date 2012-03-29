/* * 
 * * This file is part of NetEmbryo
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

#include <config.h>
#include <stdio.h>
#include <string.h>
#include "wsocket.h"
#if HAVE_SSL
#include <openssl/ssl.h>
#endif


Sock * Sock_connect(char *host, char *port, Sock *binded, sock_type socktype, sock_flags ssl_flag)
{
    Sock *s;
    char remote_host[128]; /*Unix Domain is largest*/
    char local_host[128]; /*Unix Domain is largest*/
    int sockfd = -1;
    struct sockaddr *sa_p = NULL;
    socklen_t sa_len = 0;
    int32_t local_port;
    int32_t remote_port;
#if HAVE_SSL
    SSL *ssl_con;
#endif

    if(binded) {
        sockfd = binded->fd;
    }

    if (sock_connect(host, port, &sockfd, socktype)) {
            net_log(NET_LOG_ERR, "Sock_connect() failure.\n");
            return NULL;
    }

#if HAVE_SSL
    if(ssl_flag & IS_SSL) {
        if (sock_SSL_connect(&ssl_con))
            net_log (NET_LOG_ERR, "Sock_connect() failure in SSL init.\n");
            sock_close(sockfd);
            return NULL;
    }
    else
#endif


    if (binded) {
        s = binded;
        free(s->local_host);
        s->local_host = NULL;
        free(s->remote_host);
        s->remote_host = NULL;
    } else if (!(s = calloc(1, sizeof(Sock)))) {
        net_log(NET_LOG_FATAL, "Unable to allocate a Sock struct in Sock_connect().\n");
#if HAVE_SSL
        if(ssl_flag & IS_SSL) 
            sock_SSL_close(ssl_con);
#endif
        sock_close (sockfd);
        return NULL;
    }

    s->fd = sockfd;
    s->socktype = socktype;
#if HAVE_SSL
    if(ssl_flag & IS_SSL) 
        s->ssl = ssl_con;
#endif
    s->flags = ssl_flag;

    sa_p = (struct sockaddr *) &(s->local_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getsockname(s->fd, sa_p, &sa_len))
    {
        net_log(NET_LOG_ERR, "Unable to get remote port in Sock_connect().\n");
        Sock_close(s);
        return NULL;
    }

    if(!sock_ntop_host(sa_p, local_host, sizeof(local_host)))
        memset(local_host, 0, sizeof(local_host));

    if (!(s->local_host = strdup(local_host))) {
        net_log(NET_LOG_FATAL, "Unable to allocate local host in Sock_connect().\n");
        Sock_close(s);
        return NULL;
    }

    local_port = sock_get_port(sa_p);

    if(local_port < 0) {
        net_log(NET_LOG_ERR, "Unable to get local port in Sock_connect().\n");
        Sock_close(s);
        return NULL;
    } else
        s->local_port = ntohs(local_port);

    sa_p = (struct sockaddr *) &(s->remote_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getpeername(s->fd, sa_p, &sa_len))
    {
        net_log(NET_LOG_ERR, "Unable to get remote address in Sock_connect().\n");
        Sock_close(s);
        return NULL;
    }

    if(!sock_ntop_host(sa_p, remote_host, sizeof(remote_host)))
        memset(remote_host, 0, sizeof(remote_host));
    
    if (!(s->remote_host = strdup(remote_host))) {
        net_log(NET_LOG_FATAL, "Unable to allocate remote host in Sock_connect().\n");
        Sock_close(s);
        return NULL;
    }

    remote_port = sock_get_port(sa_p);
    if(remote_port < 0) {
        net_log(NET_LOG_ERR, "Unable to get remote port in Sock_connect().\n");
        Sock_close(s);
        return NULL;
    } else
        s->remote_port = ntohs(remote_port);

    net_log(NET_LOG_DEBUG, "Socket connected between local=\"%s\":%u and "
        "remote=\"%s\":%u.\n", s->local_host, s->local_port, s->remote_host,
        s->remote_port);

    if(is_multicast_address(sa_p, s->remote_stg.ss_family)) {
        //fprintf(stderr,"IS MULTICAST\n");
        if(mcast_join(s->fd, sa_p, NULL, 0, &(s->addr))!=0) {
            Sock_close(s);
            return NULL;
        }
        s->flags |= IS_MULTICAST;
    }

    return s;
}
