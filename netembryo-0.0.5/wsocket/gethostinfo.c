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

#define HAVE_IPV4	1
#define HAVE_IPV6	1

static int
validate_family (int family)
{
  /* FIXME: Support more families. */
#if HAVE_IPV4
     if (family == PF_INET)
       return 1;
#endif
#if HAVE_IPV6
     if (family == PF_INET6)
       return 1;
#endif
     if (family == PF_UNSPEC)
       return 1;
     return 0;
}



/* Translate name of a service location and/or a service name to set of
   socket addresses. */
int
s_getaddrinfo(const char * nodename,
	     const char * servname,
	     const struct addrinfo * hints,
	     struct addrinfo ** res)
{
  struct addrinfo *tmp;
  int port = 0;
  struct hostent *he;
  void *storage;
  size_t size;
#if HAVE_IPV6
  struct v6_pair {
    struct addrinfo addrinfo;
    struct sockaddr_in6 sockaddr_in6;
  };
#endif
#if HAVE_IPV4
  struct v4_pair {
    struct addrinfo addrinfo;
    struct sockaddr_in sockaddr_in;
  };
#endif

#ifdef WIN32_NATIVE
  if (use_win32_p ())
    return getaddrinfo_ptr (nodename, servname, hints, res);
#endif

  if (hints && (hints->ai_flags & ~(AI_CANONNAME|AI_PASSIVE))) {
    /* FIXME: Support more flags. */
    return EAI_BADFLAGS;
  }

  if (hints && !validate_family (hints->ai_family)) {
    return EAI_FAMILY;
  }

  if (hints &&
      hints->ai_socktype != SOCK_STREAM && hints->ai_socktype != SOCK_DGRAM) {
    /* FIXME: Support other socktype. */
    return EAI_SOCKTYPE; /* FIXME: Better return code? */
  }

  if (!nodename)
    {
      if (!(hints->ai_flags & AI_PASSIVE)) {
        return EAI_NONAME;
      }

#ifdef HAVE_IPV6
      nodename = (hints->ai_family == AF_INET6) ? "::" : "0.0.0.0";
#else
      nodename = "0.0.0.0";
#endif
    }

  if (servname)
    {
      struct servent *se = NULL;
      const char *proto =
	(hints && hints->ai_socktype == SOCK_DGRAM) ? "udp" : "tcp";

      if (hints == NULL || !(hints->ai_flags & AI_NUMERICSERV))
	/* FIXME: Use getservbyname_r if available. */
	se = getservbyname (servname, proto);

      if (!se)
	{
	  char *c;
	  if (!(*servname >= '0' && *servname <= '9')) {
	    return EAI_NONAME;
	  }
	  port = strtoul (servname, &c, 10);
	  if (*c || port > 0xffff) {
	    return EAI_NONAME;
	  }
	  port = htons (port);
	}
      else
	port = se->s_port;
    }

  /* FIXME: Use gethostbyname_r if available. */
  he = gethostbyname (nodename);
  if (!he || he->h_addr_list[0] == NULL) {
    return EAI_NONAME;
  }

  switch (he->h_addrtype)
    {
#if HAVE_IPV6
    case PF_INET6:
      size = sizeof (struct v6_pair);
      break;
#endif

#if HAVE_IPV4
    case PF_INET:
      size = sizeof (struct v4_pair);
      break;
#endif

    default:
      return EAI_NODATA;
    }

  storage = calloc (1, size);
  if (!storage) {
    return EAI_MEMORY;
  }

  switch (he->h_addrtype)
    {
#if HAVE_IPV6
    case PF_INET6:
      {
	struct v6_pair *p = storage;
	struct sockaddr_in6 *sinp = &p->sockaddr_in6;
	tmp = &p->addrinfo;

	if (port)
	  sinp->sin6_port = port;

	if (he->h_length != sizeof (sinp->sin6_addr))
	  {
	    free (storage);
	    return EAI_SYSTEM; /* FIXME: Better return code?  Set errno? */
	  }

	memcpy (&sinp->sin6_addr, he->h_addr_list[0], sizeof sinp->sin6_addr);

	tmp->ai_addr = (struct sockaddr *) sinp;
	tmp->ai_addrlen = sizeof *sinp;
      }
      break;
#endif

#if HAVE_IPV4
    case PF_INET:
      {
	struct v4_pair *p = storage;
	struct sockaddr_in *sinp = &p->sockaddr_in;
	tmp = &p->addrinfo;

	if (port)
	  sinp->sin_port = port;

	if (he->h_length != sizeof (sinp->sin_addr))
	  {
	    free (storage);
	    return EAI_SYSTEM; /* FIXME: Better return code?  Set errno? */
	  }

	memcpy (&sinp->sin_addr, he->h_addr_list[0], sizeof sinp->sin_addr);

	tmp->ai_addr = (struct sockaddr *) sinp;
	tmp->ai_addrlen = sizeof *sinp;
      }
      break;
#endif

    default:
      free (storage);
      return EAI_NODATA;
    }

  if (hints && hints->ai_flags & AI_CANONNAME)
    {
      const char *cn;
      if (he->h_name)
	cn = he->h_name;
      else
	cn = nodename;

      tmp->ai_canonname = strdup (cn);
      if (!tmp->ai_canonname)
	{
	  free (storage);
	  return EAI_MEMORY;
	}
    }

  tmp->ai_protocol = (hints) ? hints->ai_protocol : 0;
  tmp->ai_socktype = (hints) ? hints->ai_socktype : 0;
  tmp->ai_addr->sa_family = he->h_addrtype;
  tmp->ai_family = he->h_addrtype;

  /* FIXME: If more than one address, create linked list of addrinfo's. */

  *res = tmp;

  return 0;
}


int gethostinfo(struct addrinfo **res, char *host, char *serv, struct addrinfo *hints)
{
    int n;

    if ((n = s_getaddrinfo(host, serv, hints, res)) != 0)
        return n;

    return 0;
}
