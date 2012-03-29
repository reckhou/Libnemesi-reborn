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


/*return values:
 * 0 --> false
 * 1 --> true
 * -1 -> unknown
 * */

//int16_t is_multicast_address(int sockfd)
int16_t is_multicast_address(const struct sockaddr *stg, sa_family_t family)
{
    /*sa_family_t family;
    family = sockfd_to_family(sockfd);
    struct sockaddr_storage stg;
    socklen_t len;
    
    len=sizeof(struct sockaddr_storage);

    if(getsockname(sockfd,(struct sockaddr *)&stg,&len) < 0)
        return -1;
    */
    
    switch (family) {
        case AF_INET: {
            struct in_addr *in;
            in = &(((struct sockaddr_in *) stg)->sin_addr);
            return IN_IS_ADDR_MULTICAST(ntohl(in->s_addr));
        }
#ifdef  IPV6
        case AF_INET6: {
            struct in6_addr *in6;
            in6 = &(((struct sockaddr_in6 *) stg)->sin6_addr);
            return IN6_IS_ADDR_MULTICAST(in6);
        }
#endif
#ifdef  AF_UNIX
        case AF_UNIX:
            return -1;
#endif
#ifdef  HAVE_SOCKADDR_DL_STRUCT
        case AF_LINK: 
            return -1;
#endif
        default:
            return -1;
    }
    
    return -1;
}

