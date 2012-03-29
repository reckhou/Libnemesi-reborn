/* *
 *
 *
 *  This file is part of NetEmbryo
 *
 *  NetEmbryo -- default network wrapper
 *
 *  Copyright (C) 2006 by
 *
 *      - Dario Gallucci    <dario.gallucci@gmail.com>
 *
 *  NetEmbryo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NetEmbryo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NetEmbryo; if not, write to the Free Software
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

int Sock_set_dest(Sock *s, struct sockaddr *sa) {

    if (!s)
        return -1;

    if (s->socktype != UDP) {
        net_log(NET_LOG_FATAL, "Only UDP socket can change destination address\n");
        return -1;
    }

    switch (sa->sa_family) {
    case AF_INET:
        memcpy(&(s->remote_stg), sa, sizeof(struct sockaddr_in));
        break;
    case AF_INET6:
        memcpy(&(s->remote_stg), sa, sizeof(struct sockaddr_in6));
        break;
    default:
        break;
    }
    return 0;
}
