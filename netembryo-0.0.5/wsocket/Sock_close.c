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

#include "wsocket.h"

int Sock_close(Sock *s)
{
    int res;

    if (!s)
        return -1;

    if(s->flags & IS_MULTICAST) {
        if (s->remote_host)
            mcast_leave(s->fd,(struct sockaddr *) &(s->remote_stg));
        else
            mcast_leave(s->fd,(struct sockaddr *) &(s->local_stg));
    }

#if HAVE_SSL
    if(s->flags & IS_SSL)
        sock_SSL_close(s->ssl);
#endif

    res = sock_close(s->fd);

    if (s->remote_host) free(s->remote_host);
    if (s->local_host) free(s->local_host);
    free(s);

    return res;
}
