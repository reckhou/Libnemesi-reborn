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

#if HAVE_SSL
#include <openssl/ssl.h>
#endif

#include "wsocket.h"

int Sock_create_ssl_connection(Sock *s)
{
    if(!s)
        return WSOCK_ERROR;

#if HAVE_SSL
    if(s->flags & IS_SSL) {
        s->ssl = get_ssl_connection(s->fd);
        if(!(s->ssl))
            return WSOCK_ERROR;
    }
#endif
    return WSOCK_OK;
}
