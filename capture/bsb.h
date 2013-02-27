/******************************************************************************/
/* bsb.h  -- Byte Safe Buffers
 *
 * Simple macros that deal with byte safe importing/exporting and deal with
 * buffer overflows.
 *
 */


#ifndef _BSB_HEADER
#define _BSB_HEADER

#define BSB_INIT(b, buffer, size)                 \
do {                                              \
    (b).buf = (unsigned char*)buffer;             \
    (b).ptr = (unsigned char*)buffer;             \
    int s = (int)size;                            \
    if ((buffer == NULL) || (s < 0))              \
        (b).end = 0;                              \
    else                                          \
        (b).end = (unsigned char*)buffer + size;  \
} while (0)

#define BSB_IS_ERROR(b)  ((b).end == NULL)
#define BSB_NOT_ERROR(b) ((b).end != NULL)
#define BSB_LENGTH(b)    ((b).ptr - (b).buf)
#define BSB_REMAINING(b) ((b).end?(b).end-(b).ptr:0)
#define BSB_WORK_PTR(b)  ((b).ptr)

#define BSB_EXPORT_u08(b, x)                      \
do {                                              \
    if ((b).ptr + 1 <= (b).end) {                 \
        *(((b).ptr)++) = (unsigned char)x;        \
    } else                                        \
        (b).end = 0;                              \
} while (0)

#define BSB_EXPORT_u16(b, x)                      \
do {                                              \
    if ((b).ptr + 2 <= (b).end) {                 \
        uint16_t t = (uint16_t)x;                 \
        *(((b).ptr)++) = (t & 0xff00) >> 8;       \
        *(((b).ptr)++) = (t & 0x00ff);            \
    } else                                        \
        (b).end = 0;                              \
} while (0)

#define BSB_EXPORT_u32(b, x)                      \
do {                                              \
    if ((b).ptr + 4 <= (b).end) {                 \
        uint32_t t = x;                           \
        *(((b).ptr)++) = (t & 0xff000000) >> 24;  \
        *(((b).ptr)++) = (t & 0x00ff0000) >> 16;  \
        *(((b).ptr)++) = (t & 0x0000ff00) >> 8;   \
        *(((b).ptr)++) = (t & 0x000000ff);        \
    } else                                        \
        (b).end = 0;                              \
} while (0)

#define BSB_EXPORT_ptr(b, x, size)                \
do {                                              \
    if ((b).ptr + size <= (b).end) {              \
        memcpy((b).ptr, x, size);                 \
        (b).ptr += size;                          \
    } else                                        \
        (b).end = 0;                              \
} while (0)

#define BSB_EXPORT_skip(b, size)                  \
do {                                              \
    if ((b).ptr + size <= (b).end) {              \
        (b).ptr += size;                          \
    } else                                        \
        (b).end = 0;                              \
} while (0)

#define BSB_EXPORT_rewind(b, size)                \
do {                                              \
    if ((b).ptr - size >= (b).buf) {              \
        (b).ptr -= size;                          \
    } else {                                      \
        (b).end = 0;                              \
    }                                             \
} while (0)

#if defined (C9X)

#define BSB_EXPORT_sprintf(b,...)                 \
do {                                              \
    if ((b).end != 0) {                           \
        int l = snprintf((char*)(b).ptr,          \
                         (b).end - (b).ptr,       \
                         __VA_ARGS__);            \
        if (l <= (b).end - (b).ptr) {             \
            (b).ptr += l;                         \
        } else {                                  \
            (b).end = 0;                          \
        }                                         \
    }                                             \
} while (0)

#else

#define BSB_EXPORT_sprintf(b,args...)             \
do {                                              \
    if ((b).end != 0) {                           \
        int l = snprintf((char*)(b).ptr,          \
                         (b).end - (b).ptr,       \
                         ##args);                 \
        if (l <= (b).end - (b).ptr) {             \
            (b).ptr += l;                         \
        } else {                                  \
            (b).end = 0;                          \
        }                                         \
    }                                             \
} while (0)
#endif

#define BSB_IMPORT_u08(b, x)                      \
do {                                              \
    if ((b).ptr + 1 <= (b).end) {                 \
        x = *(((b).ptr)++);                       \
    } else                                        \
        (b).end = 0;                              \
} while (0)

#define BSB_IMPORT_u16(b, x)                      \
do {                                              \
    if ((b).ptr + 2 <= (b).end) {                 \
        x = (uint16_t)(((b).ptr)[0] << 8 |        \
            ((b).ptr)[1]);                        \
        (b).ptr += 2;                             \
    } else                                        \
        (b).end = 0;                              \
} while (0)

#define BSB_IMPORT_u32(b, x)                      \
do {                                              \
    if ((b).ptr + 4 <= (b).end) {                 \
        x = (uint32_t)(((b).ptr)[0] << 24 |       \
            ((b).ptr)[1] << 16            |       \
            ((b).ptr)[2] << 8             |       \
            ((b).ptr)[3]);                        \
        (b).ptr += 4;                             \
    } else                                        \
        (b).end = 0;                              \
} while (0)

#define BSB_IMPORT_ptr(b, x, size)                \
do {                                              \
    if ((b).ptr + size <= (b).end) {              \
        (x) = (b).ptr;                            \
        (b).ptr += size;                          \
    } else {                                      \
        (b).end = 0;                              \
        x = 0;                                    \
    }                                             \
} while (0)

#define BSB_IMPORT_skip BSB_EXPORT_skip
#define BSB_IMPORT_rewind BSB_EXPORT_rewind

#define BSB_IMPORT_zbyte(b, x, size)              \
do {                                              \
    if ((b).ptr + size <= (b).end) {              \
        memcpy(x, b, size);                       \
        (x)[size] = 0;                            \
        (b).ptr += size;                          \
    } else {                                      \
        (b).end = 0;                              \
        (x)[0] = 0;                               \
    }                                             \
} while (0)

/* Private data structure */
typedef struct bsb {
    unsigned char *buf;
    unsigned char *ptr;
    unsigned char *end;
} BSB;

#endif