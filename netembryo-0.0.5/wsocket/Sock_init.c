/* *
 *  This file is part of NetEmbryo
 *
 * NetEmbryo -- default network wrapper
 *
 *  Copyright (C) 2007 by LScube team <team@streaming.polito.it
 *  See AUTHORS for more informations
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
#include <stdarg.h>

#if HAVE_SSL
#include <openssl/ssl.h>
#endif
//Shamelessly ripped from ffmpeg

static void net_log_default(int level, const char *fmt, va_list args)
{
    switch (level) {
        case NET_LOG_FATAL:
            fprintf(stderr, "[fatal error] ");
            break;
        case NET_LOG_ERR:
            fprintf(stderr, "[error] ");
            break;
        case NET_LOG_WARN:
            fprintf(stderr, "[warning] ");
            break;
        case NET_LOG_DEBUG:
#ifdef DEBUG
            fprintf(stderr, "[debug] ");
#else
            return;
#endif
            break;
        case NET_LOG_VERBOSE:
#ifdef VERBOSE
            fprintf(stderr, "[verbose debug] ");
#else
            return;
#endif
            break;
        case NET_LOG_INFO:
            fprintf(stderr, "[info] ");
            break;
        default:
            fprintf(stderr, "[unk] ");
            break;
    }

    vfprintf(stderr, fmt, args);
}

static void (*net_vlog)(int, const char*, va_list) = net_log_default;

void net_log(int level, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    net_vlog(level, fmt, vl);
    va_end(vl);
}

void Sock_init(void (*log_func)(int, const char*, va_list))
{
#if HAVE_SSL
    SSL_library_init();
    SSL_load_error_strings();
#endif

    if (log_func)
        net_vlog = log_func;

    return;
}
