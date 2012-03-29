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

#include "wsocket.h"

int Sock_socketpair(Sock *pair[]) {

    int sdpair[2], i, res;

    if (!pair)
        return -1;

    if ((res = socketpair(AF_UNIX, SOCK_DGRAM, 0, sdpair)) < 0) {
        net_log(NET_LOG_ERR, "Sock_socketpair() failure.\n");
        return res;
    }
    
    if (!(pair[0] = calloc(1, sizeof(Sock)))) {
        net_log(NET_LOG_FATAL, "Unable to allocate first Sock struct in Sock_socketpair().\n");
        close (sdpair[0]);
        close (sdpair[1]);
        return -1;
    }
    if (!(pair[1] = calloc(1, sizeof(Sock)))) {
        net_log(NET_LOG_FATAL, "Unable to allocate second Sock struct in Sock_socketpair().\n");
        close (sdpair[0]);
        close (sdpair[1]);
        free(pair[0]);
        return -1;
    }

    for (i = 0; i < 2; i++) {
        pair[i]->fd = sdpair[i];
        pair[i]->socktype = LOCAL;
    }
    
    return res;
}
