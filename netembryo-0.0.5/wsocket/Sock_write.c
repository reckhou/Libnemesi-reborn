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

int Sock_write(Sock *s, void *buffer, int nbytes, void *protodata, int flags)
{
#ifdef HAVE_LIBSCTP
    struct sctp_sndrcvinfo sinfo;
#endif

    if (!s)
        return -1;

#if HAVE_SSL
    if(s->flags & IS_SSL)
        return sock_SSL_write(s->ssl, buffer, nbytes);
    else {
#endif        
        switch (s->socktype) {
        case TCP:
            return send(s->fd, buffer, nbytes, flags);
            break;
        case UDP:
            if (!protodata) {
                protodata = &(s->remote_stg);
            }
            return sendto(s->fd, buffer, nbytes, flags, (struct sockaddr *) 
                    protodata, sizeof(struct sockaddr_storage));
            break;
        case SCTP:
#ifdef HAVE_LIBSCTP
            if (!protodata) {
                protodata = &sinfo;
                memset(protodata, 0, sizeof(struct sctp_sndrcvinfo));
            }
            return sctp_send(s->fd, buffer, nbytes, 
                (struct sctp_sndrcvinfo *) protodata, flags);
#endif
            break;
        case LOCAL:
            return send(s->fd, buffer, nbytes, flags);
            break;
        default:
            break;
        }
#if HAVE_SSL
    }
#endif        
    return -1;
}
