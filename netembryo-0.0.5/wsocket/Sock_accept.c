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
#include <sys/types.h>
#include <string.h>
#include "wsocket.h"
#if HAVE_SSL
#include <openssl/ssl.h>
#endif

Sock * Sock_accept(Sock *s)
{
    int res = -1;
    char remote_host[128]; /*Unix Domain is largest*/
    char local_host[128]; /*Unix Domain is largest*/
    int32_t remote_port = -1;
    int32_t local_port = -1;
    Sock *new_s = NULL;
    struct sockaddr *sa_p = NULL;
    socklen_t sa_len = 0;
#if HAVE_SSL
    SSL *ssl_con = NULL;
#endif

    if (!s)
        return NULL;

    if ((res = sock_accept(s->fd)) < 0) {
        net_log(NET_LOG_ERR, "System error in sock_accept().\n");
        return NULL;
    }

#if HAVE_SSL
    if(s->flags & IS_SSL) {
        if(sock_SSL_accept(&ssl_con,res)) {
            net_log(NET_LOG_ERR, "Unable to accept SSL connection.\n");
            sock_close(res);
            return NULL;
        }
    }
#endif

    if (!(new_s = calloc(1, sizeof(Sock)))) {
        net_log(NET_LOG_FATAL, "Unable to allocate a Sock struct in Sock_accept().\n");
#if HAVE_SSL
        if(s->flags & IS_SSL) 
            sock_SSL_close(ssl_con);
#endif
        sock_close(res);
        return NULL;
    }

    new_s->fd = res;
    new_s->socktype = s->socktype;
    new_s->flags = s->flags;

#if HAVE_SSL
    if(s->flags & IS_SSL) 
        new_s->ssl = ssl_con;
#endif

    sa_p = (struct sockaddr *) &(new_s->remote_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getpeername(res, sa_p, &sa_len))
    {
        net_log(NET_LOG_ERR, "Unable to get remote address in Sock_accept().\n");
        Sock_close(new_s);
        return NULL;
    }

    if(!sock_ntop_host(sa_p, remote_host, sizeof(remote_host)))
        memset(remote_host, 0, sizeof(remote_host));

    if (!(new_s->remote_host = strdup(remote_host))) {
        net_log(NET_LOG_FATAL, "Unable to allocate remote host in Sock_accept().\n");
        Sock_close(new_s);
        return NULL;
    }

    remote_port = sock_get_port(sa_p);
    if(remote_port < 0) {
        net_log(NET_LOG_ERR, "Unable to get remote port in Sock_accept().\n");
        Sock_close(new_s);
        return NULL;
    }
    else
        new_s->remote_port = ntohs(remote_port);

    sa_p = (struct sockaddr *) &(new_s->remote_stg);
    sa_len = sizeof(struct sockaddr_storage);

    if(getsockname(res, sa_p, &sa_len))
    {
        net_log(NET_LOG_ERR, "Unable to get remote port in Sock_accept().\n");
        Sock_close(new_s);
        return NULL;
    }

    if(!sock_ntop_host(sa_p, local_host, sizeof(local_host)))
        memset(local_host, 0, sizeof(local_host));

    if (!(new_s->local_host = strdup(local_host))) {
        net_log(NET_LOG_FATAL, "Unable to allocate local host in Sock_accept().\n");
        Sock_close(new_s);
        return NULL;
    }

    local_port = sock_get_port(sa_p);
    if(local_port < 0) {
        net_log(NET_LOG_ERR, "Unable to get local port in Sock_accept().\n");
        Sock_close(new_s);
        return NULL;
    }
    else
        new_s->local_port = ntohs(local_port);

    net_log(NET_LOG_DEBUG, "Socket accepted between local=\"%s\":%u and "
        "remote=\"%s\":%u.\n", new_s->local_host, new_s->local_port,
        new_s->remote_host, new_s->remote_port);

    return new_s;
}
