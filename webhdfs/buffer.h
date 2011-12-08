#ifndef _BUFFER_H_
#define _BUFFER_H_

typedef struct buffer buffer_t;

struct buffer {
    unsigned char *blob;
    size_t         block;
    size_t         size;
};

int         buffer_open             (buffer_t *buffer);
void        buffer_close            (buffer_t *buffer);

void        buffer_clear            (buffer_t *buffer);

int         buffer_reserve          (buffer_t *buffer,
                                     size_t reserve);

int         buffer_append           (buffer_t *buffer,
                                     const void *blob,
                                     size_t size);
int         buffer_append_format    (buffer_t *buffer,
                                     const char *frmt,
                                     ...);
int         buffer_append_vformat   (buffer_t *buffer,
                                     const char *frmt,
                                     va_list ap);

int         buffer_compare          (const buffer_t *buffer,
                                     const void *blob,
                                     size_t size);

#endif /* !_BUFFER_H_ */

