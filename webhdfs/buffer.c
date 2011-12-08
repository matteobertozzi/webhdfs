#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "buffer.h"

#define __align_up(x, align)            ((((x) + ((align) - 1)) & (-(align))))

static int __u64tostr (uint64_t value, char *nptr, int base) {
    char buffer[64];
    unsigned int i;
    unsigned int n;
    int c;

    if (value == 0) {
        nptr[0] = '0';
        return(1);
    }

    for (n = 0; value != 0; ++n) {
        c = (value % base) & 0xFF;
        value /= base;
        buffer[n] = (c > 9) ? (c - 10 + 'a') : (c + '0');
    }

    n--;
    for (i = 0; i <= n; ++i)
        nptr[i] = buffer[n - i];

    return(n + 1);
}

static int __i64tostr (int64_t value, char *nptr, int base) {
    int n = 0;

    if (value < 0) {
        value = -value;
        nptr[0] = '-';
        nptr++;
        n = 1;
    }

    return(n + __u64tostr((uint64_t)value, nptr, base));
}

static int __buffer_grow (buffer_t *buffer, size_t size) {
    unsigned char *blob;

    size = __align_up(size, 128);
    if ((blob = (unsigned char *) realloc(buffer->blob, size)) != NULL) {
        buffer->blob = blob;
        buffer->block = size;
        return(0);
    }

    return(1);
}

int buffer_open (buffer_t *buffer) {
    buffer->blob = NULL;
    buffer->block = 0U;
    buffer->size = 0U;
    return(0);
}

void buffer_close (buffer_t *buffer) {
    if (buffer->blob != NULL) {
        free(buffer->blob);
        buffer->blob = NULL;
    }

    buffer->block = 0U;
    buffer->size = 0U;
}

void buffer_clear (buffer_t *buffer) {
    buffer->size = 0;
}

int buffer_reserve (buffer_t *buffer, size_t reserve) {
    if (reserve > buffer->block) {
        if (__buffer_grow(buffer, reserve))
            return(1);
    }
    return(0);
}

int buffer_append (buffer_t *buffer, const void *blob, size_t size) {
    size_t n;

    if ((n = (buffer->size + size)) >= buffer->block) {
        if (__buffer_grow(buffer, n))
            return(1);
    }

    memcpy(buffer->blob + buffer->size, blob, size);
    buffer->blob[n] = '\0';
    buffer->size = n;

    return(0);
}

int buffer_append_format (buffer_t *buffer, const char *frmt, ...) {
    va_list ap;
    int r;

    va_start(ap, frmt);
    r = buffer_append_vformat(buffer, frmt, ap);
    va_end(ap);

    return(r);
}

int buffer_append_vformat (buffer_t *buffer, const char *frmt, va_list ap) {
    char nbuf[68];
    int base;
    char *p;
    char c;
    int n;

    while ((c = *frmt++) != '\0') {
        if (c != '%') {
            if (buffer_append(buffer, &c, 1))
                return(1);
            continue;
        }

        switch ((c = *frmt++)) {
            case 's':
                if ((p = va_arg(ap, char *)) == NULL)
                    p = "(none)";

                if (buffer_append(buffer, p, strlen(p)))
                    return(2);
                break;
            case 'c':
                c = (char)va_arg(ap, int);
            case '%':
                if (buffer_append(buffer, &c, 1))
                    return(4);
                break;
            case 'l':
            case 'd':
            case 'x':
            case 'o':
            case 'b':
                switch (c == 'l' ? *frmt : c) {
                    case 'd': base = 10; break;
                    case 'x': base = 16; break;
                    case 'o': base = 8;  break;
                    case 'b': base = 2;  break;
                    default:  base = 10; break;
                }

                if (c != 'l') {
                    n = __i64tostr(va_arg(ap, int32_t), nbuf, base);
                } else {
                    n = __i64tostr(va_arg(ap, int64_t), nbuf, base);
                    frmt++;
                }

                if (buffer_append(buffer, nbuf, n))
                    return(3);
                break;
        }
    }

    return(0);
}

int buffer_compare (const buffer_t *buffer, const void *blob, size_t size) {
    size_t min_size;
    int cmp;

    min_size = (buffer->size < size) ? buffer->size : size;
    if ((cmp = memcmp(buffer->blob, blob, min_size)))
        return(cmp);

    return(buffer->size - size);
}

