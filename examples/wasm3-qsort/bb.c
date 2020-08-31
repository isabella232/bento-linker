////// AUTOGENERATED //////
#include <m3_api_defs.h>
#include <m3_env.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <wasm3.h>

//// box imports ////

int box_qsort(uint32_t *buffer, size_t size);

//// box exports ////

extern void POWER_CLOCK_IRQHandler(void);

extern void TIMER0_IRQHandler(void);

extern ssize_t __box_write(int32_t a0, const void *a1, size_t size);

//// box hooks ////

// Initialize box qsort. Resets the box to its initial state if already
// initialized.
int __box_qsort_init(void);

// Mark the box qsort as needing to be reinitialized.
int __box_qsort_clobber(void);

// Allocate size bytes on the box's data stack. May return NULL if a stack
// overflow would occur.
void *__box_qsort_push(size_t size);

// Deallocate size bytes on the box's data stack.
void __box_qsort_pop(size_t size);

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

// wams3 supports sharing of the M3Environment state
IM3Environment __box_wasm3_environment;

__attribute__((unused))
static int __box_wasm3_toerr(M3Result res) {
    // note we can't use switch/case here because these are pointers
    if      (res == m3Err_none)                             return 0;
    // general errors
    else if (res == m3Err_typeListOverflow)                 return -ENOMEM;
    else if (res == m3Err_mallocFailed)                     return -ENOMEM;
    // parse errors
    else if (res == m3Err_incompatibleWasmVersion)          return -ENOEXEC;
    else if (res == m3Err_wasmMalformed)                    return -ENOEXEC;
    else if (res == m3Err_misorderedWasmSection)            return -ENOEXEC;
    else if (res == m3Err_wasmUnderrun)                     return -ENOEXEC;
    else if (res == m3Err_wasmOverrun)                      return -ENOEXEC;
    else if (res == m3Err_wasmMissingInitExpr)              return -ENOEXEC;
    else if (res == m3Err_lebOverflow)                      return -ENOEXEC;
    else if (res == m3Err_missingUTF8)                      return -ENOEXEC;
    else if (res == m3Err_wasmSectionUnderrun)              return -ENOEXEC;
    else if (res == m3Err_wasmSectionOverrun)               return -ENOEXEC;
    else if (res == m3Err_invalidTypeId)                    return -ENOEXEC;
    else if (res == m3Err_tooManyMemorySections)            return -ENOEXEC;
    // link errors
    else if (res == m3Err_moduleAlreadyLinked)              return -ENOEXEC;
    else if (res == m3Err_functionLookupFailed)             return -ENOEXEC;
    else if (res == m3Err_functionImportMissing)            return -ENOEXEC;
    else if (res == m3Err_malformedFunctionSignature)       return -ENOEXEC;
    else if (res == m3Err_funcSignatureMissingReturnType)   return -ENOEXEC;
    // compilation errors
    else if (res == m3Err_noCompiler)                       return -ENOEXEC;
    else if (res == m3Err_unknownOpcode)                    return -ENOEXEC;
    else if (res == m3Err_functionStackOverflow)            return -EOVERFLOW;
    else if (res == m3Err_functionStackUnderrun)            return -ENOEXEC;
    else if (res == m3Err_mallocFailedCodePage)             return -ENOMEM;
    else if (res == m3Err_settingImmutableGlobal)           return -ENOEXEC;
    else if (res == m3Err_optimizerFailed)                  return -ENOEXEC;
    // runtime errors
    else if (res == m3Err_missingCompiledCode)              return -ENOEXEC;
    else if (res == m3Err_wasmMemoryOverflow)               return -ENOEXEC;
    else if (res == m3Err_globalMemoryNotAllocated)         return -ENOEXEC;
    else if (res == m3Err_globaIndexOutOfBounds)            return -ENOEXEC;
    else if (res == m3Err_argumentCountMismatch)            return -ENOEXEC;
    // traps
    else if (res == m3Err_trapOutOfBoundsMemoryAccess)      return -EFAULT;
    else if (res == m3Err_trapDivisionByZero)               return -EDOM;
    else if (res == m3Err_trapIntegerOverflow)              return -ERANGE;
    else if (res == m3Err_trapIntegerConversion)            return -ERANGE;
    else if (res == m3Err_trapIndirectCallTypeMismatch)     return -ENOEXEC;
    else if (res == m3Err_trapTableIndexOutOfRange)         return -EFAULT;
    else if (res == m3Err_trapTableElementIsNull)           return -EFAULT;
    else if (res == m3Err_trapExit)                         return -ECANCELED;
    else if (res == m3Err_trapAbort)                        return -ECANCELED;
    else if (res == m3Err_trapUnreachable)                  return -EFAULT;
    else if (res == m3Err_trapStackOverflow)                return -EOVERFLOW;
    // fallback to general error?
    else                                                    return -EGENERAL;
}

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

__attribute__((noreturn))
void __box_abort(int err) {
    // if there's no other course of action, we spin
    while (1) {}
}

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

int __box_flush(int32_t fd) {
    return 0;
}

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

//// ISR Vector definitions ////

extern void main(void);

// Reset Handler
__attribute__((naked, noreturn))
int32_t __box_reset_handler(void) {
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


    // FPU bringup?
    #if defined(__VFP_FP__) && !defined(__SOFTFP__)
    #define CPACR ((volatile uint32_t*)0xe000ed88)
    *CPACR |= 0x00f00000;
    __asm__ volatile ("dsb");
    __asm__ volatile ("isb");
    #endif

    // init libc
    extern void __libc_init_array(void);
    __libc_init_array();

    // enter main
    main();

    // halt if main exits
    while (1) {
        __asm__ volatile ("wfi");
    }
}

//// Default handlers ////

__attribute__((naked, noreturn))
void __box_nmi_handler(void) {
    while (1) {}
}

__attribute__((naked, noreturn))
void __box_hardfault_handler(void) {
    while (1) {}
}

__attribute__((naked, noreturn))
void __box_memmanage_handler(void) {
    while (1) {}
}

__attribute__((naked, noreturn))
void __box_busfault_handler(void) {
    while (1) {}
}

__attribute__((naked, noreturn))
void __box_usagefault_handler(void) {
    while (1) {}
}

__attribute__((naked, noreturn))
void __box_svc_handler(void) {
    while (1) {}
}

__attribute__((naked, noreturn))
void __box_debugmon_handler(void) {
    while (1) {}
}

__attribute__((naked, noreturn))
void __box_pendsv_handler(void) {
    while (1) {}
}

__attribute__((naked, noreturn))
void __box_systick_handler(void) {
    while (1) {}
}

__attribute__((naked, noreturn))
void __box_default_handler(void) {
    while (1) {}
}



extern uint32_t __stack_end;

//// ISR Vector ////

__attribute__((used, section(".isr_vector")))
const uint32_t __isr_vector[256] = {
    (uint32_t)&__stack_end,
    (uint32_t)&__box_reset_handler,
    // Exception handlers
    (uint32_t)__box_nmi_handler,
    (uint32_t)__box_hardfault_handler,
    (uint32_t)__box_memmanage_handler,
    (uint32_t)__box_busfault_handler,
    (uint32_t)__box_usagefault_handler,
    (uint32_t)0,
    (uint32_t)0,
    (uint32_t)0,
    (uint32_t)0,
    (uint32_t)__box_svc_handler,
    (uint32_t)__box_debugmon_handler,
    (uint32_t)0,
    (uint32_t)__box_pendsv_handler,
    (uint32_t)__box_systick_handler,
    // External IRQ handlers
    (uint32_t)POWER_CLOCK_IRQHandler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)TIMER0_IRQHandler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
};

//// qsort loading ////

static int __box_qsort_load(void) {
    // default loader does nothing
    return 0;
}

//// qsort state ////
bool __box_qsort_initialized = false;
IM3Runtime __box_qsort_runtime;
IM3Module __box_qsort_module;
uint32_t __box_qsort_datasp;

__attribute__((unused))
static uint32_t __box_qsort_fromptr(const void *ptr) {
    return (uint32_t)((const uint8_t*)ptr
        - m3MemData(__box_qsort_runtime->memory.mallocated));
}

__attribute__((unused))
static void *__box_qsort_toptr(uint32_t ptr) {
    return m3MemData(__box_qsort_runtime->memory.mallocated) + ptr;
}

void *__box_qsort_push(size_t size) {
    // we maintain a separate stack in the wasm memory space,
    // sharing the stack space of the wasm-side libc
    uint32_t psp = __box_qsort_datasp;
    if (psp + size > 49152) {
        return NULL;
    }

    __box_qsort_datasp = psp + size;
    return __box_qsort_toptr(psp);
}

void __box_qsort_pop(size_t size) {
    assert(__box_qsort_datasp - size >= 0);
    __box_qsort_datasp -= size;
}

m3ApiRawFunction(__box_qsort_import___box_qsort_abort) {
    m3ApiGetArg(int, err);
    __box_qsort_runtime->exit_code = err;
    m3ApiTrap(m3Err_trapExit);
}

// redirect __box_qsort_write -> __box_write
#define __box_qsort_write __box_write

// redirect __box_qsort_flush -> __box_flush
#define __box_qsort_flush __box_flush

//// qsort imports ////

m3ApiRawFunction(__box_qsort_import___box_qsort_write) {
    m3ApiReturnType(ssize_t);
    m3ApiGetArg(int32_t, a0);
    m3ApiGetArgMem(const void*, a1);
    m3ApiGetArg(size_t, a2);
    ssize_t r0 = __box_qsort_write(a0, a1, a2);
    m3ApiReturn(r0);
}

m3ApiRawFunction(__box_qsort_import___box_qsort_flush) {
    m3ApiReturnType(int);
    m3ApiGetArg(int32_t, a0);
    int r0 = __box_qsort_flush(a0);
    m3ApiReturn(r0);
}

//// qsort exports ////

int box_qsort(uint32_t *buffer, size_t size) {
    if (!__box_qsort_initialized) {
        int err = __box_qsort_init();
        if (err) {
            return err;
        }
    }

    M3Result res;
    IM3Function f;
    res = m3_FindFunction(&f,
            __box_qsort_runtime,
            "box_qsort");
    if (res || !f->compiled ||
            f->funcType->numArgs != 2) {
        return -ENOEXEC;
    }

    uint64_t *stack = __box_qsort_runtime->stack;
    *(uint32_t*)&stack[0] = __box_qsort_fromptr(buffer);
    *(size_t*)&stack[1] = size;
    m3StackCheckInit();
    res = (M3Result)Call(
            f->compiled,
            (m3stack_t)stack,
            __box_qsort_runtime->memory.mallocated,
            d_m3OpDefaultArgs);
    if (res) {
        if (res == m3Err_trapExit) {
            return __box_qsort_runtime->exit_code;
        }
        return __box_wasm3_toerr(res);
    }
    return *(int*)&stack[0];
}

//// qsort init ////

int __box_qsort_init(void) {
    int err;
    if (__box_qsort_initialized) {
        return 0;
    }

    // load the box if unloaded
    err = __box_qsort_load();
    if (err) {
        return err;
    }

    // initialize wasm3 environment, this only needs
    // to be done once
    if (!__box_wasm3_environment) {
        __box_wasm3_environment = m3_NewEnvironment();
        if (!__box_wasm3_environment) {
            return -ENOMEM;
        }
    }

    // initialize wasm3 runtime
    __box_qsort_runtime = m3_NewRuntime(
            __box_wasm3_environment,
            4096,
            NULL);
    if (!__box_qsort_runtime) {
        return -ENOMEM;
    }
    extern uint32_t __box_qsort_image;
    M3Result res;
    res = m3_ParseModule(
            __box_wasm3_environment,
            &__box_qsort_module,
            (const uint8_t*)(&__box_qsort_image + 1),
            __box_qsort_image);
    if (res) {
        return __box_wasm3_toerr(res);
    }

    res = m3_LoadModule(__box_qsort_runtime, __box_qsort_module);
    if (res) {
        return __box_wasm3_toerr(res);
    }

    // link imports
    res = m3_LinkRawFunction(
            __box_qsort_module,
            "*",
            "__box_abort",
            "v(i)",
            __box_qsort_import___box_qsort_abort);
    if (res && res != m3Err_functionLookupFailed) {
        return __box_wasm3_toerr(res);
    }
    res = m3_LinkRawFunction(
            __box_qsort_module,
            "*",
            "__box_write",
            "i(i*i)",
            __box_qsort_import___box_qsort_write);
    if (res && res != m3Err_functionLookupFailed) {
        return __box_wasm3_toerr(res);
    }
    res = m3_LinkRawFunction(
            __box_qsort_module,
            "*",
            "__box_flush",
            "i(i)",
            __box_qsort_import___box_qsort_flush);
    if (res && res != m3Err_functionLookupFailed) {
        return __box_wasm3_toerr(res);
    }

    // setup data stack, note address 0 is NULL
    // so we can't start there!
    __box_qsort_datasp = 4;

    __box_qsort_initialized = true;
    return 0;
}

int __box_qsort_clobber(void) {
    if (__box_qsort_initialized) {
        m3_FreeRuntime(__box_qsort_runtime);
    }
    __box_qsort_initialized = false;
    return 0;
}

