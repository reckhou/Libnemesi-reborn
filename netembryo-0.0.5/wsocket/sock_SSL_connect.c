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

#include <stdio.h>
#include <openssl/ssl.h>
#include "wsocket.h"

int sock_SSL_connect(SSL **ssl_con, int sockfd)
{
    int ssl_err;
    SSL_CTX *ssl_ctx = NULL;

    ssl_ctx = SSL_CTX_new(SSLv3_client_method());
    if(!ssl_ctx) {
        net_log(NET_LOG_ERR, "sock_SSL_connect: !ssl_ctx\n");
        return WSOCK_ERROR;
    }

    *ssl_con = SSL_new(ssl_ctx);

    if(!(*ssl_con)) {
        net_log(NET_LOG_ERR, "sock_SSL_connect: SSL_new() failed.\n");
        SSL_CTX_free(ssl_ctx);
        return WSOCK_ERROR;
    }
    
    SSL_set_fd (*ssl_con, sockfd);
    SSL_set_connect_state(*ssl_con);
    ssl_err = SSL_connect(*ssl_con);

    if(ssl_err < 0) 
        SSL_set_shutdown(*ssl_con,SSL_SENT_SHUTDOWN);
    if(ssl_err <= 0) {
        net_log(NET_LOG_ERR, "sock_SSL_connect: SSL_connect() failed.\n");
        SSL_free(*ssl_con);
        SSL_CTX_free(ssl_ctx);
        return WSOCK_ERROR;
    }

    return WSOCK_OK;
}
