/**
 * @file io.c
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Implements printf
 *
 */
#include <bl/bios.h>
#include <bl/io.h>
#include <bl/string.h>

/* Leave this undocumented */
#ifndef DOX_SKIP

/* Write function prototype */
typedef void (*buffer_fcn)(void* buffer,
                           size_t max_size,
                           size_t index,
                           char ch);

/* Write to memory buffer */
static void
_buffer_mem(void* buffer, size_t max_size, size_t index, char ch) {
    if (index < max_size)
        ((char*)buffer)[index] = ch;
}

/* Write to terminal */
static void
_buffer_out(void* buffer, size_t max_size, size_t index, char ch) {
    (void)buffer;
    (void)max_size;
    (void)index;

    if (ch == '\n') {
        bios_putch('\r');
        bios_putch('\n');
    } else if (ch != '\0') {
        bios_putch(ch);
    }
}

/* Write to COM port */
static void
_buffer_serial(void* buffer, size_t max_size, size_t index, char ch) {
    (void)buffer;
    (void)max_size;
    (void)index;

    if (ch == '\n') {
        bios_serial_putch('\r');
        bios_serial_putch('\n');
    } else {
        bios_serial_putch(ch);
    }
}

/* Don't write */
static void
_buffer_null(void* buffer, size_t max_size, size_t index, char ch) {
    (void)buffer;
    (void)max_size;
    (void)index;
    (void)ch;
}

/* Format flags */
#define FLAG_LEFT (1U << 0)
#define FLAG_SIGN (1U << 1)
#define FLAG_SPACE (1U << 2)
#define FLAG_HASH (1U << 3)
#define FLAG_ZERO (1U << 4)

/* Precision flag */
#define FLAG_PRECISION (1U << 5)

/* Size flags */
#define FLAG_CHAR (1U << 6)
#define FLAG_SHORT (1U << 7)
#define FLAG_LONG (1U << 8)
#define FLAG_LONG_LONG (1U << 9)
#define FLAG_SIZE (1U << 10)
#define FLAG_PTRDIFF (1U << 11)
#define FLAG_INTMAX (1U << 12)

/* Writes formatted output */
static size_t
_format_output(buffer_fcn write,
               void* buffer,
               size_t max_size,
               size_t index,
               const char* output,
               size_t output_size,
               uint16_t flags,
               uint16_t width) {
    size_t i;
    char fill = flags & FLAG_ZERO ? '0' : ' ';

    if (flags & FLAG_LEFT) {
        /* Left-Justification*/

        /* Write output string first */
        for (i = 0U; i < output_size; ++i)
            write(buffer, max_size, index++, *output++);

        /* Pad if output_size < width */
        for (; i < width; ++i)
            write(buffer, max_size, index++, fill);
    } else {
        /* Right-Justification */

        /* Calculate fill size, assuming width is unsigned (if output_size >
         * width then output_size
         * - width will be incorrect) */
        width = width < output_size ? 0 : width - output_size;

        /* Pad if calculated fill size > 0 */
        for (i = 0U; i < width; ++i)
            write(buffer, max_size, index++, fill);

        /* Write output string */
        for (i = 0U; i < output_size; ++i)
            write(buffer, max_size, index++, *output++);
    }

    return index;
}

/* Reverse string */
static void
_reverse(char* buff, size_t buff_size) {
    size_t i, j;
    char c;

    for (i = 0, j = buff_size - 1; i < j; ++i, --j) {
        c = buff[i];
        buff[i] = buff[j];
        buff[j] = c;
    }
}

/* Hexademical ASCII alphabet */
static char hcase_alphabet[] = "0123456789ABCDEF";
static char lcase_alphabet[] = "0123456789abcdef";

/* Converts number to formatted string */
static size_t
_ntoa(uintmax_t num,
      bool negative,
      uint8_t radix,
      char* num_buffer,
      uint16_t flags,
      uint16_t precision,
      bool hcase) {
    size_t index = 0U;

    /* Write digits in reverse sequence */
    do {
        num_buffer[index++] =
          (hcase ? hcase_alphabet : lcase_alphabet)[num % radix];
    } while ((num /= radix) > 0);

    /* Pad number if precision > index */
    if (flags & FLAG_PRECISION)
        while (index < precision)
            num_buffer[index++] = '0';

    /* Add prefix */
    if (flags & FLAG_HASH) {
        if (radix == 16) {
            num_buffer[index++] = hcase ? 'X' : 'x';
        }
        if (radix % 8 == 0)
            num_buffer[index++] = '0';
    }

    /* Write sign */
    if (negative)
        num_buffer[index++] = '-';
    else if (flags & FLAG_SIGN)
        num_buffer[index++] = '+';
    else if (flags & FLAG_SPACE)
        num_buffer[index++] = ' ';

    /* Reverse buffer */
    _reverse(num_buffer, index);

    return index;
}

/* Internal vsnprintf */
static int
_vsnprintf(void* buffer,
           size_t max_size,
           const char* format,
           va_list va,
           buffer_fcn write) {
    size_t index = 0U, num_size;
    uint16_t flags, width, precision;
    bool flags_loop;
    uint8_t radix;
    char num_buffer[32];
    union {
        char char_type;
        unsigned char uchar_type;
        short short_type;
        unsigned short ushort_type;
        int int_type;
        unsigned int uint_type;
        long long_type;
        unsigned long ulong_type;
        long long long_long_type;
        unsigned long long ulong_long_type;
        size_t size_type;
        ptrdiff_t ptrdiff_type;
        intmax_t intmax_type;
        uintmax_t uintmax_type;
        uintptr_t ptr_type;
    } arg;

    if (!buffer)
        write = _buffer_null;

    while (*format) {
        if (*format != '%') {
            write(buffer, max_size, index++, *format++);
        } else {
            format++;

            /* Parse flags */
            flags = 0U;
            flags_loop = true;
            while (flags_loop) {
                switch (*format) {
                    case '-':
                        flags |= FLAG_LEFT;
                        format++;
                        break;
                    case '+':
                        flags |= FLAG_SIGN;
                        format++;
                        break;
                    case ' ':
                        flags |= FLAG_SPACE;
                        format++;
                        break;
                    case '#':
                        flags |= FLAG_HASH;
                        format++;
                        break;
                    case '0':
                        flags |= FLAG_ZERO;
                        format++;
                        break;
                    default:
                        flags_loop = false;
                }
            }

            /* Parse width */
            width = 0U;
            if (*format == '*') {
                arg.int_type = va_arg(va, int);
                if (arg.int_type < 0) {
                    flags |= FLAG_LEFT;
                    width = -arg.int_type;
                } else {
                    width = arg.int_type;
                }
                format++;
            } else {
                while ('0' <= *format && *format <= '9')
                    width = (width * 10) + (*format++ - '0');
            }

            /* Parse precision */
            precision = 0U;
            if (*format == '.') {
                flags |= FLAG_PRECISION;
                format++;
                if (*format == '*') {
                    arg.int_type = va_arg(va, int);
                    precision = arg.int_type < 0 ? 0 : arg.int_type;
                    format++;
                } else {
                    while ('0' <= *format && *format <= '9')
                        precision = (precision * 10) + (*format++ - '0');
                }
            }

            /* Parse length */
            switch (*format) {
                case 'h':
                    flags |= FLAG_SHORT;
                    format++;
                    if (*format == 'h') {
                        flags |= FLAG_CHAR;
                        format++;
                    }
                    break;
                case 'l':
                    flags |= FLAG_LONG;
                    format++;
                    if (*format == 'l') {
                        flags |= FLAG_LONG_LONG;
                        format++;
                    }
                    break;
                case 'j':
                    flags |= FLAG_INTMAX;
                    format++;
                    break;
                case 'z':
                    flags |= FLAG_SIZE;
                    format++;
                    break;
                case 't':
                    flags |= FLAG_PTRDIFF;
                    format++;
                    break;
            }

            /* Parse specifier and format output */
            switch (*format) {
                case 'i':
                case 'd': {
                    arg.intmax_type = 0U;
                    if (flags & FLAG_CHAR)
                        arg.char_type = (char)va_arg(va, int);
                    else if (flags & FLAG_SHORT)
                        arg.short_type = (short)va_arg(va, int);
                    else if (flags & FLAG_LONG_LONG)
                        arg.long_long_type = va_arg(va, long long);
                    else if (flags & FLAG_LONG)
                        arg.long_type = va_arg(va, long);
                    else if (flags & FLAG_INTMAX)
                        arg.intmax_type = va_arg(va, intmax_t);
                    else if (flags & FLAG_SIZE)
                        arg.size_type = va_arg(va, size_t);
                    else if (flags & FLAG_PTRDIFF)
                        arg.ptrdiff_type = va_arg(va, ptrdiff_t);
                    else
                        arg.int_type = va_arg(va, int);

                    num_size = _ntoa(arg.intmax_type < 0 ? -arg.intmax_type
                                                         : arg.intmax_type,
                                     arg.intmax_type < 0,
                                     10,
                                     num_buffer,
                                     flags,
                                     precision,
                                     false);
                    index = _format_output(write,
                                           num_buffer,
                                           max_size,
                                           index,
                                           num_buffer,
                                           num_size,
                                           flags,
                                           width);

                    format++;
                } break;
                case 'u':
                case 'o':
                case 'x':
                case 'X': {
                    arg.uintmax_type = 0U;
                    if (flags & FLAG_CHAR)
                        arg.uchar_type =
                          (unsigned char)va_arg(va, unsigned int);
                    else if (flags & FLAG_SHORT)
                        arg.ushort_type =
                          (unsigned short)va_arg(va, unsigned int);
                    else if (flags & FLAG_LONG_LONG)
                        arg.ulong_long_type = va_arg(va, unsigned long long);
                    else if (flags & FLAG_LONG)
                        arg.ulong_type = va_arg(va, unsigned long);
                    else if (flags & FLAG_INTMAX)
                        arg.uintmax_type = va_arg(va, uintmax_t);
                    else if (flags & FLAG_SIZE)
                        arg.size_type = va_arg(va, size_t);
                    else if (flags & FLAG_PTRDIFF)
                        arg.ptrdiff_type = va_arg(va, ptrdiff_t);
                    else
                        arg.uint_type = va_arg(va, unsigned int);

                    switch (*format) {
                        case 'u':
                            radix = 10;
                            break;
                        case 'o':
                            radix = 8;
                            break;
                        case 'x':
                        case 'X':
                            radix = 16;
                            break;
                    }

                    num_size = _ntoa(arg.uintmax_type,
                                     false,
                                     radix,
                                     num_buffer,
                                     flags,
                                     precision,
                                     *format == 'X' ? true : false);
                    index = _format_output(write,
                                           buffer,
                                           max_size,
                                           index,
                                           num_buffer,
                                           num_size,
                                           flags,
                                           width);

                    format++;
                } break;
                case 'c': {
                    arg.intmax_type = 0U;
                    if (flags & FLAG_LONG)
                        /* WINT_T won't be implemented */;
                    else
                        arg.int_type = va_arg(va, int);

                    index = _format_output(write,
                                           buffer,
                                           max_size,
                                           index,
                                           (const char*)&arg.int_type,
                                           1,
                                           flags,
                                           width);

                    format++;
                } break;
                case 's': {
                    arg.ptr_type = (uintptr_t)va_arg(va, void*);
                    if (flags & FLAG_LONG)
                        /* WCHAR_T won't be implemented */;
                    else
                        index =
                          _format_output(write,
                                         buffer,
                                         max_size,
                                         index,
                                         (const char*)arg.ptr_type,
                                         strlen((const char*)arg.ptr_type),
                                         flags,
                                         width);

                    format++;
                } break;
                case 'p': {
                    arg.uintmax_type = 0U;
                    arg.ptr_type = (uintptr_t)va_arg(va, void*);

                    flags |= FLAG_HASH;
                    num_size = _ntoa(arg.uintmax_type,
                                     false,
                                     16,
                                     num_buffer,
                                     flags,
                                     precision,
                                     false);
                    index = _format_output(write,
                                           buffer,
                                           max_size,
                                           index,
                                           num_buffer,
                                           num_size,
                                           flags,
                                           width);

                    format++;
                } break;
                case 'n': {
                    arg.ptr_type = (uintptr_t)va_arg(va, void*);
                    if (flags & FLAG_CHAR)
                        *(char*)arg.ptr_type = index;
                    else if (flags & FLAG_SHORT)
                        *(short*)arg.ptr_type = index;
                    else if (flags & FLAG_LONG_LONG)
                        *(long long*)arg.ptr_type = index;
                    else if (flags & FLAG_LONG)
                        *(long*)arg.ptr_type = index;
                    else if (flags & FLAG_INTMAX)
                        *(intmax_t*)arg.ptr_type = index;
                    else if (flags & FLAG_SIZE)
                        *(size_t*)arg.ptr_type = index;
                    else if (flags & FLAG_PTRDIFF)
                        *(ptrdiff_t*)arg.ptr_type = index;
                    else
                        *(int*)arg.ptr_type = index;

                    format++;
                } break;
                case '%': {
                    index = _format_output(write,
                                           buffer,
                                           max_size,
                                           index,
                                           format++,
                                           1,
                                           flags,
                                           width);
                } break;
                default:
                    format++;
            }
        }
    }

    /* Place null-terminator */
    write(buffer, max_size, index < max_size ? index : max_size - 1, '\0');
    return index;
}

#endif /* DOX_SKIP */

int
vsnprintf(char* s, size_t n, const char* format, va_list arg) {
    return _vsnprintf(s, n, format, arg, _buffer_mem);
}

int
snprintf(char* s, size_t n, const char* format, ...) {
    va_list va;
    int ret;

    va_start(va, format);
    ret = _vsnprintf(s, n, format, va, _buffer_mem);
    va_end(va);

    return ret;
}

int
sprintf(char* s, const char* format, ...) {
    va_list va;
    int ret;

    va_start(va, format);
    ret = _vsnprintf(s, SIZE_MAX, format, va, _buffer_mem);
    va_end(va);

    return ret;
}

int
printf(const char* format, ...) {
    va_list va;
    char buffer[1];
    int ret;

    va_start(va, format);
    ret = _vsnprintf(buffer, SIZE_MAX, format, va, _buffer_out);
    va_end(va);

    return ret;
}

int
serial_printf(const char* format, ...) {
    va_list va;
    char buffer[1];
    int ret;

    va_start(va, format);
    ret = _vsnprintf(buffer, SIZE_MAX, format, va, _buffer_serial);
    va_end(va);

    return ret;
}
