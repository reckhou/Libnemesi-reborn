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

#include "wsocket.h"

#ifdef HAVE_SOCKADDR_DL_STRUCT
#include <net/if_dl.h>
#endif

#if !defined(WIN32) && defined(AF_UNIX)
# include <sys/un.h>
#endif

const char *sock_ntop_host(const struct sockaddr *sa, char *str, size_t len)
{
    switch (sa->sa_family) {
    case AF_INET: {
        struct sockaddr_in    *sin = (struct sockaddr_in *) sa;
        return(inet_ntop(AF_INET, &(sin->sin_addr), str, len));
    }

#ifdef    IPV6
    case AF_INET6: {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
        int a = 0;
        char *tmp = str;
        const char *retval = inet_ntop(AF_INET6, &(sin6->sin6_addr), str, len);
        if (retval) {
            while ((tmp = strchr(tmp, '.'))) {
                a++;
                tmp++;
            }
            if (a == 3) {
                if (!strncmp(str, "::ffff:", 7)) {
                    //this is an IPv4 address mapped in IPv6 address space
                    memmove (str, &str[7], strlen(str) - 6); // one char more for trailing NUL char
                } else {
                    //this is an IPv6 address containg an IPv4 address (like ::127.0.0.1)
                    memmove (str, &str[2], strlen(str) - 1);
                }
            }
        }
        return retval;
    }
#endif

#if !defined(WIN32) && defined(AF_UNIX)
    case AF_UNIX: {
        struct sockaddr_un *unp = (struct sockaddr_un *) sa;

            /* OK to have no pathname bound to the socket: happens on
               every connect() unless client calls bind() first. */
        if (unp->sun_path[0] == '\0')
            strncpy(str, "(no pathname bound)", len);
        else
            strncpy(str, unp->sun_path, len);
        return(str);
    }
#endif

#ifdef    HAVE_SOCKADDR_DL_STRUCT
    case AF_LINK: {
        struct sockaddr_dl *sdl = (struct sockaddr_dl *) sa;
        /*if (sdl->sdl_nlen > 0)
            snprintf(str, len, "%*s",
                     sdl->sdl_nlen, &sdl->sdl_data[0]);
        else
            snprintf(str, len, "AF_LINK, index=%d", sdl->sdl_index);*/
        return(str);
    }
#endif
    default:
        break;
    }
    return (NULL);
}


