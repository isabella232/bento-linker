////// AUTOGENERATED //////
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

//// box exports ////

extern int32_t boxc_add2(int32_t a0, int32_t a1);

extern int boxc_fib(uint32_t *buffer, size_t size, uint32_t a, uint32_t b);

extern void* boxc_fib_alloc(size_t size);

extern int boxc_fib_next(uint32_t *next, uint32_t a, uint32_t b);

extern int boxc_hello(void);

extern int boxc_qsort(uint32_t *buffer, size_t size);

extern void* boxc_qsort_alloc(size_t size);

extern ssize_t boxc_qsort_partition(uint32_t *buffer, size_t size, uint32_t pivot);

// May be called by well-behaved code to terminate the box if execution can
// not continue. Notably used for asserts. Note that __box_abort may be
// skipped if the box is killed because of an illegal operation. Must not
// return.
__attribute__((noreturn))
void __box_abort(int err);

// Provides a minimal implementation of stdout to the box. The exact behavior
// depends on the superbox's implementation of __box_write. If none is
// provided, __box_write links but does nothing.
ssize_t __box_write(int32_t fd, const void *buffer, size_t size);

// Provides a minimal implementation of stdout to the box. The exact behavior
// depends on the superbox's implementation of __box_flush. If none is
// provided, __box_flush links but does nothing.
int __box_flush(int32_t fd);

//// box error codes ////
enum box_errors {
    EOK              = 0,    // No error
    EGENERAL         = 1,    // General error
    ENOENT           = 2,    // No such file or directory
    ESRCH            = 3,    // No such process
    EINTR            = 4,    // Interrupted system call
    EIO              = 5,    // I/O error
    ENXIO            = 6,    // No such device or address
    E2BIG            = 7,    // Argument list too long
    ENOEXEC          = 8,    // Exec format error
    EBADF            = 9,    // Bad file number
    ECHILD           = 10,   // No child processes
    EAGAIN           = 11,   // Try again
    ENOMEM           = 12,   // Out of memory
    EACCES           = 13,   // Permission denied
    EFAULT           = 14,   // Bad address
    EBUSY            = 16,   // Device or resource busy
    EEXIST           = 17,   // File exists
    EXDEV            = 18,   // Cross-device link
    ENODEV           = 19,   // No such device
    ENOTDIR          = 20,   // Not a directory
    EISDIR           = 21,   // Is a directory
    EINVAL           = 22,   // Invalid argument
    ENFILE           = 23,   // File table overflow
    EMFILE           = 24,   // Too many open files
    ENOTTY           = 25,   // Not a typewriter
    ETXTBSY          = 26,   // Text file busy
    EFBIG            = 27,   // File too large
    ENOSPC           = 28,   // No space left on device
    ESPIPE           = 29,   // Illegal seek
    EROFS            = 30,   // Read-only file system
    EMLINK           = 31,   // Too many links
    EPIPE            = 32,   // Broken pipe
    EDOM             = 33,   // Math argument out of domain of func
    ERANGE           = 34,   // Math result not representable
    EDEADLK          = 35,   // Resource deadlock would occur
    ENAMETOOLONG     = 36,   // File name too long
    ENOLCK           = 37,   // No record locks available
    ENOSYS           = 38,   // Function not implemented
    ENOTEMPTY        = 39,   // Directory not empty
    ELOOP            = 40,   // Too many symbolic links encountered
    ENOMSG           = 42,   // No message of desired type
    EIDRM            = 43,   // Identifier removed
    ENOSTR           = 60,   // Device not a stream
    ENODATA          = 61,   // No data available
    ETIME            = 62,   // Timer expired
    ENOSR            = 63,   // Out of streams resources
    ENOLINK          = 67,   // Link has been severed
    EPROTO           = 71,   // Protocol error
    EMULTIHOP        = 72,   // Multihop attempted
    EBADMSG          = 74,   // Not a data message
    EOVERFLOW        = 75,   // Value too large for defined data type
    EILSEQ           = 84,   // Illegal byte sequence
    ENOTSOCK         = 88,   // Socket operation on non-socket
    EDESTADDRREQ     = 89,   // Destination address required
    EMSGSIZE         = 90,   // Message too long
    EPROTOTYPE       = 91,   // Protocol wrong type for socket
    ENOPROTOOPT      = 92,   // Protocol not available
    EPROTONOSUPPORT  = 93,   // Protocol not supported
    EOPNOTSUPP       = 95,   // Operation not supported on transport endpoint
    EAFNOSUPPORT     = 97,   // Address family not supported by protocol
    EADDRINUSE       = 98,   // Address already in use
    EADDRNOTAVAIL    = 99,   // Cannot assign requested address
    ENETDOWN         = 100,  // Network is down
    ENETUNREACH      = 101,  // Network is unreachable
    ENETRESET        = 102,  // Network dropped connection because of reset
    ECONNABORTED     = 103,  // Software caused connection abort
    ECONNRESET       = 104,  // Connection reset by peer
    ENOBUFS          = 105,  // No buffer space available
    EISCONN          = 106,  // Transport endpoint is already connected
    ENOTCONN         = 107,  // Transport endpoint is not connected
    ETIMEDOUT        = 110,  // Connection timed out
    ECONNREFUSED     = 111,  // Connection refused
    EHOSTUNREACH     = 113,  // No route to host
    EALREADY         = 114,  // Operation already in progress
    EINPROGRESS      = 115,  // Operation now in progress
    ESTALE           = 116,  // Stale NFS file handle
    EDQUOT           = 122,  // Quota exceeded
    ECANCELED        = 125,  // Operation Canceled
    EOWNERDEAD       = 130,  // Owner died
    ENOTRECOVERABLE  = 131,  // State not recoverable
};

#if defined(__GNUC__)
// state of brk
static uint8_t *__heap_brk = NULL;
// assigned by linker
extern uint8_t __heap_start;
extern uint8_t __heap_end;

// GCC's _sbrk uses sp for bounds checking, this
// does not work if our stack is located before the heap
void *_sbrk(ptrdiff_t diff) {
    if (!__heap_brk) {
        __heap_brk = &__heap_start;
    }

    uint8_t *pbrk = __heap_brk;
    if (pbrk + diff > &__heap_end) {
        return (void*)-1;
    }

    __heap_brk = pbrk+diff;
    return pbrk;
}
#endif

//// __box_abort glue ////

__attribute__((used))
__attribute__((noreturn))
void __wrap_abort(void) {
    __box_abort(-1);
}

__attribute__((used))
void __wrap_exit(int code) {
    __box_abort(code > 0 ? -code : code);
}

#if defined(__GNUC__)
__attribute__((noreturn))
void __assert_func(const char *file, int line,
        const char *func, const char *expr) {
    printf("%s:%d: assertion \"%s\" failed\n", file, line, expr);
    __box_abort(-1);
}

__attribute__((noreturn))
void _exit(int code) {
    __box_abort(code > 0 ? -code : code);
}
#endif

//// __box_write glue ////

ssize_t __box_cbprintf(
        ssize_t (*write)(void *ctx, const void *buf, size_t size), void *ctx,
        const char *format, va_list args) {
    const char *p = format;
    ssize_t res = 0;
    while (true) {
        // first consume everything until a '%'
        size_t skip = strcspn(p, "%");
        if (skip > 0) {
            ssize_t nres = write(ctx, p, skip);
            if (nres < 0) {
                return nres;
            }
            res += nres;
        }

        p += skip;

        // hit end of string?
        if (!*p) {
            return res;
        }

        // format parser
        bool zero_justify = false;
        bool left_justify = false;
        bool precision_mode = false;
        size_t width = 0;
        size_t precision = 0;

        char mode = 'c';
        uint32_t value = 0;
        size_t size = 0;

        for (;; p++) {
            if (p[1] >= '0' && p[1] <= '9') {
                // precision/width
                if (precision_mode) {
                    precision = precision*10 + (p[1]-'0');
                } else if (p[1] > '0' || width > 0) {
                    width = width*10 + (p[1]-'0');
                } else {
                    zero_justify = true;
                }

            } else if (p[1] == '*') {
                // dynamic precision/width
                if (precision_mode) {
                    precision = va_arg(args, size_t);
                } else {
                    width = va_arg(args, size_t);
                }

            } else if (p[1] == '.') {
                // switch mode
                precision_mode = true;

            } else if (p[1] == '-') {
                // left-justify
                left_justify = true;

            } else if (p[1] == '%') {
                // single '%'
                mode = 'c';
                value = '%';
                size = 1;
                break;

            } else if (p[1] == 'c') {
                // char
                mode = 'c';
                value = va_arg(args, int);
                size = 1;
                break;

            } else if (p[1] == 's') {
                // string
                mode = 's';
                const char *s = va_arg(args, const char *);
                value = (uint32_t)s;
                // find size, don't allow overruns
                size = 0;
                while (s[size] && (precision == 0 || size < precision)) {
                    size += 1;
                }
                break;

            } else if (p[1] == 'd' || p[1] == 'i') {
                // signed decimal number
                mode = 'd';
                int32_t d = va_arg(args, int32_t);
                value = (uint32_t)d;
                size = 0;
                if (d < 0) {
                    size += 1;
                    d = -d;
                }
                for (uint32_t t = d; t > 0; t /= 10) {
                    size += 1;
                }
                if (size == 0) {
                    size += 1;
                }
                break;

            } else if (p[1] == 'u') {
                // unsigned decimal number
                mode = 'u';
                value = va_arg(args, uint32_t);
                size = 0;
                for (uint32_t t = value; t > 0; t /= 10) {
                    size += 1;
                }
                if (size == 0) {
                    size += 1;
                }
                break;

            } else if (p[1] >= ' ' && p[1] <= '?') {
                // unknown modifier? skip

            } else {
                // hex or unknown character, terminate

                // make it prettier for pointers
                if (!(p[1] == 'x' || p[1] == 'X')) {
                    zero_justify = true;
                    width = 2*sizeof(void*);
                }

                // hexadecimal number
                mode = 'x';
                value = va_arg(args, uint32_t);
                size = 0;
                for (uint32_t t = value; t > 0; t /= 16) {
                    size += 1;
                }
                if (size == 0) {
                    size += 1;
                }
                break;
            }
        }

        // consume the format
        p += 2;

        // format printing
        if (!left_justify) {
            for (ssize_t i = 0; i < (ssize_t)width-(ssize_t)size; i++) {
                char c = (zero_justify) ? '0' : ' ';
                ssize_t nres = write(ctx, &c, 1);
                if (nres < 0) {
                    return nres;
                }
                res += nres;
            }
        }

        if (mode == 'c') {
            ssize_t nres = write(ctx, &value, 1);
            if (nres < 0) {
                return nres;
            }
            res += nres;
        } else if (mode == 's') {
            ssize_t nres = write(ctx, (const char*)(uintptr_t)value, size);
            if (nres < 0) {
                return nres;
            }
            res += nres;
        } else if (mode == 'x') {
            for (ssize_t i = size-1; i >= 0; i--) {
                uint32_t digit = (value >> (4*i)) & 0xf;

                char c = ((digit >= 10) ? ('a'-10) : '0') + digit;
                ssize_t nres = write(ctx, &c, 1);
                if (nres < 0) {
                    return nres;
                }
                res += nres;
            }
        } else if (mode == 'd' || mode == 'u') {
            ssize_t i = size-1;

            if (mode == 'd' && (int32_t)value < 0) {
                ssize_t nres = write(ctx, "-", 1);
                if (nres < 0) {
                    return nres;
                }
                res += nres;

                value = -value;
                i -= 1;
            }

            for (; i >= 0; i--) {
                uint32_t temp = value;
                for (int j = 0; j < i; j++) {
                    temp /= 10;
                }
                uint32_t digit = temp % 10;

                char c = '0' + digit;
                ssize_t nres = write(ctx, &c, 1);
                if (nres < 0) {
                    return nres;
                }
                res += nres;
            }
        }

        if (left_justify) {
            for (ssize_t i = 0; i < (ssize_t)width-(ssize_t)size; i++) {
                char c = ' ';
                ssize_t nres = write(ctx, &c, 1);
                if (nres < 0) {
                    return nres;
                }
                res += nres;
            }
        }
    }
}

static ssize_t __box_vprintf_write(void *ctx, const void *buf, size_t size) {
    return __box_write((int32_t)ctx, buf, size);
}

__attribute__((used))
ssize_t __wrap_vprintf(const char *format, va_list args) {
    return __box_cbprintf(__box_vprintf_write, (void*)1, format, args);
}

__attribute__((used))
ssize_t __wrap_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    ssize_t res = __wrap_vprintf(format, args);
    va_end(args);
    return res;
}

__attribute__((used))
ssize_t __wrap_vfprintf(FILE *f, const char *format, va_list args) {
    int32_t fd = (f == stdout) ? 1 : 2;
    return __box_cbprintf(__box_vprintf_write, (void*)fd, format, args);
}

__attribute__((used))
ssize_t __wrap_fprintf(FILE *f, const char *format, ...) {
    va_list args;
    va_start(args, format);
    ssize_t res = __wrap_vfprintf(f, format, args);
    va_end(args);
    return res;
}

__attribute__((used))
int __wrap_fflush(FILE *f) {
    int32_t fd = (f == stdout) ? 1 : 2;
    return __box_flush(fd);
}

#if defined(__GNUC__)
int _write(int handle, const char *buffer, int size) {
    return __box_write(handle, (const uint8_t*)buffer, size);
}
#endif

int __box_init(void) {
    // load data
    extern uint32_t __data_init_start;
    extern uint32_t __data_start;
    extern uint32_t __data_end;
    const uint32_t *s = &__data_init_start;
    for (uint32_t *d = &__data_start; d < &__data_end; d++) {
        *d = *s++;
    }

    // zero bss
    extern uint32_t __bss_start;
    extern uint32_t __bss_end;
    for (uint32_t *d = &__bss_start; d < &__bss_end; d++) {
        *d = 0;
    }

    // init libc
    extern void __libc_init_array(void);
    __libc_init_array();

    return 0;
}

//// imports ////

//// exports ////

// box-side jumptable
extern uint8_t __stack_end;
__attribute__((used, section(".jumptable")))
const uint32_t __box_jumptable[] = {
    (uint32_t)&__stack_end,
    (uint32_t)__box_init,
    (uint32_t)boxc_add2,
    (uint32_t)boxc_fib,
    (uint32_t)boxc_fib_alloc,
    (uint32_t)boxc_fib_next,
    (uint32_t)boxc_hello,
    (uint32_t)boxc_qsort,
    (uint32_t)boxc_qsort_alloc,
    (uint32_t)boxc_qsort_partition,
};

