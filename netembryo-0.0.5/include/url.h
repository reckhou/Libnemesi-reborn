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

#ifndef _NETEMBRYO_URL_H_
#define _NETEMBRYO_URL_H_

#include <string.h>

/** @defgroup NetEmbryo_Url Url Management Interface
 *
 * @brief These module offers high level functions to parse and handle URLs
 *
 * @{ */

/**
 * Saved informations about a parsed url will be stored here
 */
typedef struct
{
    char * protocol; //!< The protocol specified in the url (http, rtsp, etc)
    char * hostname; //!< The hostname specified in the url (www.something.org, 192.168.0.1, etc)
    char * port; //!< The port specified in the url
    char * path; //!< The path of the specific object to access (/path/to/resource.ext)
} Url;

int Url_init(Url * url, char * urlname);
void Url_destroy(Url * url);
int Url_decode (char *decoded_string, const char *source_string, size_t decoded_string_size);
int Url_encode (char *encoded_string, const char *source_string, size_t encoded_string_size);

/**
 * @}
 */

#endif
