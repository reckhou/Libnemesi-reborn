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
 * this piece of code is taken from NeMeSI
 * */


#include <sys/types.h>
#include "wsocket.h"

int32_t sock_get_port(const struct sockaddr *sa)
{
    switch (sa->sa_family) {
        case AF_INET: {
            struct sockaddr_in    *sin = (struct sockaddr_in *) sa;
    
            return(sin->sin_port);
        }

#ifdef    IPV6
        case AF_INET6: {
            struct sockaddr_in6    *sin6 = (struct sockaddr_in6 *) sa;
    
            return(sin6->sin6_port);
        }
#endif
    }

    return -1;
}
