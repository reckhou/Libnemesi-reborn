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

#include "url.h"
#include <stdio.h>
#include <ctype.h>

/**
 * Creates an Url informations structure from a URI string
 *
 * @param url The url to initialize (Will not free the previous data,
 *            Url_destroy must be used to free it if we already initialized
 *            the structure before)
 * @param urlname The URI to parse to create the Url informations
 * @return Always 0, errors will be reported by setting to NULL the field of
 *         the Url structure that the function was not able to parse.
 */
int Url_init(Url * url, char * urlname)
{
    char * protocol_begin, * hostname_begin, * port_begin, * path_begin;
    size_t protocol_len, hostname_len, port_len, path_len;

    memset(url, 0, sizeof(Url));

    hostname_begin = strstr(urlname, "://");
    if (hostname_begin == NULL) {
        hostname_begin = urlname;
        protocol_begin = NULL;
        protocol_len = 0;
    } else {
        protocol_len = (size_t)(hostname_begin - urlname);
        hostname_begin = hostname_begin + 3;
        protocol_begin = urlname;
    }

    hostname_len = strlen(urlname) - ((size_t)(hostname_begin - urlname));

    path_begin = strstr(hostname_begin, "/");
    if (path_begin == NULL) {
        path_len = 0;
    } else {
        ++path_begin;
        hostname_len = (size_t)(path_begin - hostname_begin - 1);
        path_len = strlen(urlname) - ((size_t)(path_begin - urlname));
    }

    port_begin = strstr(hostname_begin, ":");
    if ((port_begin == NULL) ||
        ((port_begin >= path_begin) && (path_begin != NULL))) {
        port_len = 0;
        port_begin = NULL;
    } else {
        ++port_begin;
        if (path_len)
            port_len = (size_t)(path_begin - port_begin - 1);
        else
            port_len = strlen(urlname) - ((size_t)(port_begin - urlname));
        hostname_len = (size_t)(port_begin - hostname_begin - 1);
    }

    if (protocol_len) {
        url->protocol = (char*)malloc(protocol_len+1);
        strncpy(url->protocol, protocol_begin, protocol_len);
        url->protocol[protocol_len] = '\0';
    }

    if (port_len) {
        url->port = (char*)malloc(port_len+1);
        strncpy(url->port, port_begin, port_len);
        url->port[port_len] = '\0';
    }

    if (path_len) {
        url->path = (char*)malloc(path_len+1);
        strncpy(url->path, path_begin, path_len);
        url->path[path_len] = '\0';
    }

    url->hostname = (char*)malloc(hostname_len+1);
    strncpy(url->hostname, hostname_begin, hostname_len);
    url->hostname[hostname_len] = '\0';

    return 0;
}

/**
 * Will destroy the Url structure freeing the data contained in it
 *
 * @param url The Url structure to destroy
 */
void Url_destroy(Url * url)
{
    free(url->protocol);
    free(url->hostname);
    free(url->port);
    free(url->path);
}

/**
 * Converts an hex char to a decimal value
 *
 * @param data The char of an hex encoded digit
 * @return -1 if given char is not 0-F
 */
int hex_to_dec (char data)
{
    if( '0' <= data && data <= '9' ) // 0 - 9
        return data - '0';
    else if( 'A' <= data && data <= 'F' ) // A - F
        return data - 'A' + 10;
    else if( 'a' <= data && data <= 'f' ) // a - f
        return data - 'a' + 10;
    else
        return -1;
}

/**
 * Decode a url following RFC 1738
 *
 * @param decoded_string a pre-allocated string where decoded charecters will be stored
 * @param source_string the string where the encoded url is
 * @param decoded_string_size the size of decoded string char array
 * @return -1 if url is invalid, lenght of decoded string otherwise
 */
int Url_decode (char *decoded_string, const char *source_string,
                size_t decoded_string_size)
{

    int decoded_string_pos = 0, source_string_len, i, dec, unit;
    memset(decoded_string, '\0', decoded_string_size * sizeof(char));

    if (!source_string)
        return -1;

    source_string_len = strlen (source_string);
    for (i = 0;
         i < source_string_len && decoded_string_pos < decoded_string_size;
         i++ ) {
        if (source_string[i] == '%') {
            if ( (i < (source_string_len - 2)) &&
                 ( (dec = hex_to_dec(source_string[i+1]))>= 0 ) &&
                 ( (unit = hex_to_dec(source_string[i+2]))>=0 ) ) {
                     decoded_string[decoded_string_pos] =
                        (char)(dec * 16 + unit);
                     i += 2;
                     decoded_string_pos++;
                 } else {
                     return -1;
                 }
        } else if( source_string[i] == '+' ) {
            decoded_string[decoded_string_pos] = ' ';
            decoded_string_pos++;
        } else {
            decoded_string[decoded_string_pos] = source_string[i];
            decoded_string_pos++;
        }
    }
    if (i != source_string_len) {
        // if decoded_string is too short for decoded url
        return -1;
    }
    decoded_string[decoded_string_pos] = '\0';
    return decoded_string_pos;
}

/**
 * Encode a url following RFC 1738
 *
 * @param encoded_string a pre-allocated string where encoded charecters will be stored
 * @param source_string the string where the url to be encoded is
 * @param encoded_string_size the size of encoded string char array
 * @return -1 if url is invalid, lenght of encoded string otherwise
 */
int Url_encode (char *encoded_string, const char *source_string,
                size_t encoded_string_size)
{

    int encoded_string_pos = 0, source_string_len, i;
    memset(encoded_string, '\0', encoded_string_size * sizeof(char));

    if (!source_string)
        return -1;

    source_string_len = strlen (source_string);
    for (i = 0;
         i < source_string_len && encoded_string_pos < encoded_string_size;
         i++ ) {
        switch (source_string[i]) {
        case ' ':
            encoded_string[encoded_string_pos] = '+';
            encoded_string_pos++;
            break;
        case ';':
        case '+':
        case '?':
        case ':':
        case '@':
        case '&':
        case '=':
            snprintf (encoded_string + encoded_string_pos,
                      encoded_string_size - encoded_string_pos,
                      "%%%2x", source_string[i]);
            encoded_string_pos += 3;
            break;
        default:
            if ( iscntrl(source_string[i]) ) {
                return -1;
            } else {
                encoded_string[encoded_string_pos] = source_string[i];
                encoded_string_pos++;
            }
        }
    }
    if (i != source_string_len) {
        // if encoded_string is too short for encoded url
        return -1;
    }
    encoded_string[encoded_string_pos] = '\0';
    return encoded_string_pos;
}
