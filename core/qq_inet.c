/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


in_addr_t
qq_inet_addr(u_char *text, size_t len)
{
    u_char      *p, c;
    in_addr_t    addr;
    qq_uint_t    octet, n;

    addr = 0;
    octet = 0;
    n = 0;

    for (p = text; p < text + len; p++) {
        c = *p;
        if (c >= '0' && c <= '9') {
            octet = octet * 10 + (c - '0');
            if (octet > 255) {
                return INADDR_NONE;
            }

            continue;
        }

        if (c == '.') {
            addr = (addr << 8) + octet;
            octet = 0;
            n++;
            continue;
        }

        return INADDR_NONE;
    }

    if (n == 3) {
        addr = (addr << 8) + octet;
        return htonl(addr);
    }

    return INADDR_NONE;
}

#if (QQ_HAVE_INET6)
qq_int_t
qq_inet6_addr(u_char *p, size_t len, u_char *addr)
{
    u_char      c, *zero, *digit, *s, *d;
    size_t      len4;
    qq_uint_t   n, nibbles, word;

    if (len == 0) {
        return QQ_ERROR;
    }

    zero = NULL;
    digit = NULL;
    len4 = 0;
    nibbles = 0;
    word = 0;
    n = 8;

    if (p[0] == ':') {
        p++;
        len--;
    }

    for (/* void */; len; len--) {
        c = *p++;
        if (c == ':') {
            if (nibbles) {
                digit = p;
                len4 = len;
                *addr++ = (u_char) (word >> 8);
                *addr++ = (u_char) (word & 0xff);

                if (--n) {
                    nibbles = 0;
                    word = 0;
                    continue;
                }
            } else {
                if (zero == NULL) {
                    digit = p;
                    len4 = len;
                    zero = addr;
                    continue;
                }
            }
            return NGX_ERROR;
        }

        if (c == '.' && nibbles) {
            if (n < 2 || digit == NULL) {
                return QQ_ERROR;
            }

            word = qq_inet_addr(digit, len4 - 1);
            if (word == INADDR_NONE) {
                return QQ_ERROR;
            }

            word = ntohl(word);
            *addr++ = (u_char) ((word >> 24) & 0xff);
            *addr++ = (u_char) ((word >> 16) & 0xff);
            n--;
            break;
        }

        if (++nibbles > 4) {
            return QQ_ERROR;
        }

        if (c >= '0' && c <= '9') {
            word = word * 16 + (c - '0');
            continue;
        }

        c |= 0x20;

        if (c >= 'a' && c <= 'f') {
            word = word * 16 + (c - 'a') + 10;
            continue;
        }

        return QQ_ERROR;
    }

    if (nibbles == 0 && zero == NULL) {
        return QQ_ERROR;
    }

    *addr++ = (u_char) (word >> 8);
    *addr++ = (u_char) (word & 0xff);

    if (--n) {
        if (zero) {
            n *= 2;
            s = addr - 1;
            d = s + n;
            while (s >= zero) {
                *d-- = *s--;
            }
            memset(zero, 0, n);
            return QQ_OK;
        }

    } else {
        if (zero == NULL) {
            return QQ_OK;
        }
    }

    return QQ_ERROR;
}
#endif

size_t
qq_sock_ntop(struct sockaddr *sa, socklen_t socklen, u_char *text, size_t len,
    qq_uint_t port)
{
    u_char               *p;
#if (QQ_HAVE_INET6 || QQ_HAVE_UNIX_DOMAIN)
    size_t                n;
#endif
    struct sockaddr_in   *sin;
#if (QQ_HAVE_INET6)
    struct sockaddr_in6  *sin6;
#endif
#if (QQ_HAVE_UNIX_DOMAIN)
    struct sockaddr_un   *saun;
#endif

    switch (sa->sa_family) {
    case AF_INET:
        sin = (struct sockaddr_in *) sa;
        p = (u_char *) &sin->sin_addr;

        if (port) {
            p = snprintf(text, len, "%u.%u.%u.%u:%d",
                         p[0], p[1], p[2], p[3], ntohs(sin->sin_port));
        } else {
            p = snprintf(text, len, "%u.%u.%u.%u",
                         p[0], p[1], p[2], p[3]);
        }

        return (p - text);

#if (QQ_HAVE_INET6)

    case AF_INET6:
        sin6 = (struct sockaddr_in6 *) sa;
        n = 0;

        if (port) {
            text[n++] = '[';
        }

        n = qq_inet6_ntop(sin6->sin6_addr.s6_addr, &text[n], len);

        if (port) {
            n = sprintf(&text[1 + n], "]:%d",
                        ntohs(sin6->sin6_port)) - text;
        }

        return n;
#endif

#if (QQ_HAVE_UNIX_DOMAIN)

    case AF_UNIX:
        saun = (struct sockaddr_un *) sa;

        if (socklen <= (socklen_t) offsetof(struct sockaddr_un, sun_path)) {
            p = snprintf(text, len, "unix:%Z");

        } else {
            n = qq_strnlen((u_char *) saun->sun_path,
                           socklen - offsetof(struct sockaddr_un, sun_path));
            p = snprintf(text, len, "unix:%*s%Z", n, saun->sun_path);
        }

        return (p - text - 1);

#endif

    default:
        return 0;
    }
}


size_t
qq_inet_ntop(int family, void *addr, u_char *text, size_t len)
{
    u_char  *p;

    switch (family) {
    case AF_INET:
        p = addr;
        return snprintf(text, len, "%u.%u.%u.%u",
                        p[0], p[1], p[2], p[3]);
                        //-text;

#if (QQ_HAVE_INET6)

    case AF_INET6:
        return qq_inet6_ntop(addr, text, len);

#endif

    default:
        return 0;
    }
}


#if (QQ_HAVE_INET6)
size_t
ngx_inet6_ntop(u_char *p, u_char *text, size_t len)
{
    u_char      *dst;
    size_t       max, n;
    qq_uint_t    i, zero, last;

    if (len < QQ_INET6_ADDRSTRLEN) {
        return 0;
    }

    zero = (qq_uint_t) -1;
    last = (qq_uint_t) -1;
    max = 1;
    n = 0;

    for (i = 0; i < 16; i += 2) {

        if (p[i] || p[i + 1]) {
            if (max < n) {
                zero = last;
                max = n;
            }

            n = 0;
            continue;
        }

        if (n++ == 0) {
            last = i;
        }
    }

    if (max < n) {
        zero = last;
        max = n;
    }

    dst = text;
    n = 16;

    if (zero == 0) {
        if ((max == 5 && p[10] == 0xff && p[11] == 0xff)
            || (max == 6)
            || (max == 7 && p[14] != 0 && p[15] != 1))
        {
            n = 12;
        }

        *dst++ = ':';
    }

    for (i = 0; i < n; i += 2) {
        if (i == zero) {
            *dst++ = ':';
            i += (max - 1) * 2;
            continue;
        }

        dst = sprintf(dst, "%xd", p[i] * 256 + p[i + 1]);

        if (i < 14) {
            *dst++ = ':';
        }
    }

    if (n == 12) {
        dst = sprintf(dst, "%u.%u.%u.%u", p[12], p[13], p[14], p[15]);
    }

    return dst - text;
}
#endif
