////// AUTOGENERATED //////
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

//// box error codes ////
enum box_err {
    BOX_ERR_OK               = 0,    // No error
    BOX_ERR_GENERAL          = -1,   // General error
    BOX_ERR_NOBOX            = -8,   // Box format error
    BOX_ERR_AGAIN            = -11,  // Try again
    BOX_ERR_NOMEM            = -12,  // Cannot allocate memory
    BOX_ERR_FAULT            = -14,  // Bad address
    BOX_ERR_BUSY             = -16,  // Device or resource busy
    BOX_ERR_LOOP             = -20,  // Cyclic data structure detected
    BOX_ERR_INVAL            = -22,  // Invalid parameter
    BOX_ERR_TIMEDOUT         = -110, // Timed out
};

//// box imports ////

int box1_add2(int32_t a0, int32_t a1);

int box1_hello(void);

int box2_add2(int32_t a0, int32_t a1);

int box2_hello(void);

int box3_add2(int32_t a0, int32_t a1);

int box3_hello(void);

//// box exports ////

__attribute__((visibility("default"))) 
extern ssize_t __box_write(int32_t a0, void *a1, size_t a2);

//// box hooks ////

// Forcefully terminate the current box with the specified error. The box can
// not be called again after this without a new init. Does not return.
__attribute__((noreturn))
void __box_abort(int err);

// Write to stdout if provided by superbox. If not provided, this function is
// still available for linking, but does nothing. Returns 0 on success,
// negative error code on failure.
ssize_t __box_write(int32_t fd, void *buffer, size_t size);

// Initialize box box1.
int __box_box1_init(void);

// Initialize box box2.
int __box_box2_init(void);

// Initialize box box3.
int __box_box3_init(void);

//// jumptable implementation ////

struct __box_mpuregions {
    uint32_t count;
    uint32_t regions[][2];
};

#define SHCSR    ((volatile uint32_t*)0xe000ed24)
#define MPU_TYPE ((volatile uint32_t*)0xe000ed90)
#define MPU_CTRL ((volatile uint32_t*)0xe000ed94)
#define MPU_RBAR ((volatile uint32_t*)0xe000ed9c)
#define MPU_RASR ((volatile uint32_t*)0xe000eda0)

static int32_t __box_mpu_init(void) {
    // make sure MPU is initialized
    if (!(*MPU_CTRL & 0x1)) {
        // do we have an MPU?
        assert(*MPU_TYPE >= 4);
        // enable MemManage exception
        *SHCSR = *SHCSR | 0x00070000;
        // setup call region
        *MPU_RBAR = 0x1e000000 | 0x10;
        // disallow execution
        //*MPU_RASR = 0x10230021;
        *MPU_RASR = 0x10000001 | ((25-1) << 1);
        // enable the MPU
        *MPU_CTRL = 5;
    }
    return 0;
}

static void __box_mpu_switch(const struct __box_mpuregions *regions) {
    *MPU_CTRL = 0;
    uint32_t count = regions->count;
    for (int i = 0; i < 4; i++) {
        if (i < count) {
            *MPU_RBAR = (~0x1f & regions->regions[i][0]) | 0x10 | (i+1);
            *MPU_RASR = regions->regions[i][1];
        } else {
            *MPU_RBAR = 0x10 | (i+1);
            *MPU_RASR = 0;
        }
    }
    *MPU_CTRL = 5;
}

// System state
uint32_t __box_active = 0;

// Redirected __box_writes
#define __box_box1_write __box_write
#define __box_box2_write __box_write
#define __box_box3_write __box_write

// Jumptables
const uint32_t __box_sys_jumptable[] = {
    (uint32_t)NULL, // no stack for sys
    (uint32_t)__box_write,
    (uint32_t)__box_box1_write,
    (uint32_t)__box_box2_write,
    (uint32_t)__box_box3_write,
};

extern const uint32_t __box_box1_jumptable[];
extern const uint32_t __box_box2_jumptable[];
extern const uint32_t __box_box3_jumptable[];

#define __BOX_COUNT 3
const uint32_t *const __box_jumptables[__BOX_COUNT+1] = {
    __box_sys_jumptable,
    __box_box1_jumptable,
    __box_box2_jumptable,
    __box_box3_jumptable,
};

const struct __box_mpuregions __box_sys_mpuregions = {
    .count = 0,
    .regions = {},
};

const struct __box_mpuregions __box_box1_mpuregions = {
    .count = 2,
    .regions = {
        {0x000fe000, 0x02000019},
        {0x2003e000, 0x13000019},
    },
};

const struct __box_mpuregions __box_box2_mpuregions = {
    .count = 2,
    .regions = {
        {0x000fc000, 0x02000019},
        {0x2003c000, 0x13000019},
    },
};

const struct __box_mpuregions __box_box3_mpuregions = {
    .count = 2,
    .regions = {
        {0x000fa000, 0x02000019},
        {0x2003a000, 0x13000019},
    },
};

const struct __box_mpuregions *const __box_mpuregions[__BOX_COUNT+1] = {
    &__box_sys_mpuregions,
    &__box_box1_mpuregions,
    &__box_box2_mpuregions,
    &__box_box3_mpuregions,
};
__attribute__((used))
void __box_box1_fault(int err) {}
__attribute__((used))
void __box_box2_fault(int err) {}
__attribute__((used))
void __box_box3_fault(int err) {}

void (*const __box_faults[__BOX_COUNT+1])(int err) = {
    &__box_abort,
    &__box_box1_fault,
    &__box_box2_fault,
    &__box_box3_fault,
};

// Box state
struct __box_state {
    uint32_t caller;
    uint32_t lr;
    uint32_t *sp;
};

struct __box_state __box_sys_state;
struct __box_state __box_box1_state;
struct __box_state __box_box2_state;
struct __box_state __box_box3_state;

struct __box_state *const __box_state[__BOX_COUNT+1] = {
    &__box_sys_state,
    &__box_box1_state,
    &__box_box2_state,
    &__box_box3_state,
};

// Dispach logic
struct __box_frame {
    uint32_t *fp;
    uint32_t lr;
    uint32_t *sp;
};

// foward declaration of fault wrapper, may be called directly
// in other handlers, but only in other handlers! (needs isr context)
__attribute__((used, naked, noreturn))
void __box_faulthandler(int32_t err);

#define __BOX_ASSERT(test, code) do {   \
        if (!(test)) {                  \
            __box_faulthandler(code);   \
        }                               \
    } while (0)

__attribute__((used))
uint64_t __box_callsetup(uint32_t lr, uint32_t *sp,
        uint32_t op, uint32_t *fp) {
    // save lr + sp
    struct __box_state *state = __box_state[__box_active];
    struct __box_frame *frame = (struct __box_frame*)sp;
    frame->fp = fp;
    frame->lr = state->lr;
    frame->sp = state->sp;
    state->lr = lr;
    state->sp = sp;

    uint32_t path = (op & 0xffffff)/4;
    uint32_t caller = __box_active;
    __box_active = path % (__BOX_COUNT+1);
    const uint32_t *targetjumptable = __box_jumptables[__box_active];
    struct __box_state *targetstate = __box_state[__box_active];
    uint32_t targetlr = targetstate->lr;
    uint32_t *targetsp = targetstate->sp;
    // keep track of caller
    targetstate->caller = caller;
    // don't allow returns while executing
    targetstate->lr = 0;
    // need sp to fixup instruction aborts
    targetstate->sp = targetsp;
    uint32_t targetpc = targetjumptable[path / (__BOX_COUNT+1) + 1];

    // select MPU regions
    __box_mpu_switch(__box_mpuregions[__box_active]);

    // enable control?
    uint32_t control;
    __asm__ volatile ("mrs %0, control" : "=r"(control));
    control = (~1 & control) | (__box_active != 0 ? 1 : 0);
    __asm__ volatile ("msr control, %0" :: "r"(control));

    // setup new call frame
    targetsp -= 8;
    targetsp[0] = fp[0];        // r0 = arg0
    targetsp[1] = fp[1];        // r1 = arg1
    targetsp[2] = fp[2];        // r2 = arg2
    targetsp[3] = fp[3];        // r3 = arg3
    targetsp[4] = fp[4];        // r12 = r12
    targetsp[5] = 0x1f000000 + 1; // lr = __box_ret
    targetsp[6] = targetpc;     // pc = targetpc
    targetsp[7] = fp[7];        // psr = psr

    return ((uint64_t)targetlr) | ((uint64_t)(uint32_t)targetsp << 32);
}

__attribute__((used, naked, noreturn))
void __box_callhandler(uint32_t lr, uint32_t *sp, uint32_t op) {
    __asm__ volatile (
        // keep track of args
        "mov r3, r1 \n\t"
        // save core registers
        "stmdb r1!, {r4-r11} \n\t"
        // save fp registers?
        "tst r0, #0x10 \n\t"
        "it eq \n\t"
        "vstmdbeq r1!, {s16-s31} \n\t"
        // make space to save state
        "sub r1, r1, #3*4 \n\t"
        // sp == msp?
        "tst r0, #0x4 \n\t"
        "it eq \n\t"
        "moveq sp, r1 \n\t"
        // ah! reserve a frame in case we're calling this
        // interrupts stack from another stack
        "sub sp, sp, #8*4 \n\t"
        // call into c now that we have stack control
        "bl __box_callsetup \n\t"
        // update new sp
        "tst r0, #0x4 \n\t"
        "itee eq \n\t"
        "msreq msp, r1 \n\t"
        "msrne psp, r1 \n\t"
        // drop reserved frame?
        "subne sp, sp, #8*4 \n\t"
        // return to call
        "bx r0 \n\t"
    );
}

__attribute__((used))
uint64_t __box_retsetup(uint32_t lr, uint32_t *sp,
        uint32_t op, uint32_t *fp) {
    // save lr + sp
    struct __box_state *state = __box_state[__box_active];
    // drop exception frame and fixup instruction aborts
    sp = state->sp;
    state->lr = lr;
    state->sp = sp;

    __box_active = state->caller;
    struct __box_state *targetstate = __box_state[__box_active];
    uint32_t targetlr = targetstate->lr;
    __BOX_ASSERT(targetlr, BOX_ERR_FAULT); // in call?
    uint32_t *targetsp = targetstate->sp;
    struct __box_frame *targetframe = (struct __box_frame*)targetsp;
    uint32_t *targetfp = targetframe->fp;
    targetstate->lr = targetframe->lr;
    targetstate->sp = targetframe->sp;

    // select MPU regions
    __box_mpu_switch(__box_mpuregions[__box_active]);

    // enable control?
    uint32_t control;
    __asm__ volatile ("mrs %0, control" : "=r"(control));
    control = (~1 & control) | (__box_active != 0 ? 1 : 0);
    __asm__ volatile ("msr control, %0" :: "r"(control));

    // copy return frame
    targetfp[0] = fp[0];       // r0 = arg0
    targetfp[1] = fp[1];       // r1 = arg1
    targetfp[2] = fp[2];       // r2 = arg2
    targetfp[3] = fp[3];       // r3 = arg3
    targetfp[6] = targetfp[5]; // pc = lr

    return ((uint64_t)targetlr) | ((uint64_t)(uint32_t)targetsp << 32);
}

__attribute__((used, naked, noreturn))
void __box_rethandler(uint32_t lr, uint32_t *sp, uint32_t op) {
    __asm__ volatile (
        // keep track of rets
        "mov r3, r1 \n\t"
        // call into c new that we have stack control
        "bl __box_retsetup \n\t"
        // drop saved state
        "add r1, r1, #3*4 \n\t"
        // restore fp registers?
        "tst r0, #0x10 \n\t"
        "it eq \n\t"
        "vldmiaeq r1!, {s16-s31} \n\t"
        // restore core registers
        "ldmia r1!, {r4-r11} \n\t"
        // update sp
        "tst r0, #0x4 \n\t"
        "ite eq \n\t"
        "msreq msp, r1 \n\t"
        "msrne psp, r1 \n\t"
        // return
        "bx r0 \n\t"
    );
}

__attribute__((used))
uint64_t __box_faultsetup(int32_t err) {
    // invoke user handler, may not return
    // TODO should we set this up to be called in non-isr context?
    __box_faults[__box_active](err);

    struct __box_state *state = __box_state[__box_active];
    struct __box_state *targetstate = __box_state[state->caller];
    uint32_t targetlr = targetstate->lr;
    uint32_t *targetsp = targetstate->sp;
    struct __box_frame *targetbf = (struct __box_frame*)targetsp;
    uint32_t *targetfp = targetbf->fp;
    // in call?
    if (!targetlr) {
        // halt if not handled
        while (1) {}
    }

    // check if our return target supports erroring
    uint32_t op = targetfp[6];
    if (!(op & 2)) {
        // halt if not handled
        while (1) {}
    }

    // we can return an error
    __box_active = state->caller;
    targetstate->lr = targetbf->lr;
    targetstate->sp = targetbf->sp;

    // select MPU regions
    __box_mpu_switch(__box_mpuregions[__box_active]);

    // enable control?
    uint32_t control;
    __asm__ volatile ("mrs %0, control" : "=r"(control));
    control = (~1 & control) | (__box_active != 0 ? 1 : 0);
    __asm__ volatile ("msr control, %0" :: "r"(control));

    // copy return frame
    targetfp[0] = err;         // r0 = arg0
    targetfp[1] = 0;           // r1 = arg1
    targetfp[2] = 0;           // r2 = arg2
    targetfp[3] = 0;           // r3 = arg3
    targetfp[6] = targetfp[5]; // pc = lr

    return ((uint64_t)targetlr) | ((uint64_t)(uint32_t)targetsp << 32);
}

__attribute__((used, naked, noreturn))
void __box_faulthandler(int32_t err) {
    __asm__ volatile (
        // call into c with stack control
        "bl __box_faultsetup \n\t"
        // drop saved state
        "add r1, r1, #3*4 \n\t"
        // restore fp registers?
        "tst r0, #0x10 \n\t"
        "it eq \n\t"
        "vldmiaeq r1!, {s16-s31} \n\t"
        // restore core registers
        "ldmia r1!, {r4-r11} \n\t"
        // update sp
        "tst r0, #0x4 \n\t"
        "ite eq \n\t"
        "msreq msp, r1 \n\t"
        "msrne psp, r1 \n\t"
        // return
        "bx r0 \n\t"
    );
}

__attribute__((alias("__box_mpu_handler")))
void __box_usagefault_handler(void);
__attribute__((alias("__box_mpu_handler")))
void __box_busfault_handler(void);
__attribute__((alias("__box_mpu_handler")))
void __box_memmanage_handler(void);
__attribute__((naked))
void __box_mpu_handler(void) {
    __asm__ volatile (
        // get lr
        "mov r0, lr \n\t"
        "tst r0, #0x4 \n\t"
        // get sp
        "ite eq \n\t"
        "mrseq r1, msp \n\t"
        "mrsne r1, psp \n\t"
        // get pc
        "ldr r2, [r1, #6*4] \n\t"
        // call?
        "ldr r3, =#0x1e000000 \n\t"
        "sub r3, r2, r3 \n\t"
        "lsrs r3, r3, #25-1 \n\t"
        "beq __box_callhandler \n\t"
        // ret?
        "ldr r3, =#0x1f000000 \n\t"
        "subs r3, r2, r3 \n\t"
        "beq __box_rethandler \n\t"
        // fallback to fault handler
        // explicit fault?
        "ldr r3, =#0x1f000000 + 1*4 \n\t"
        "subs r3, r2, r3 \n\t"
        "ite eq \n\t"
        "ldreq r0, [r1, #0] \n\t"
        "ldrne r0, =%0 \n\t"
        "b __box_faulthandler \n\t"
        :
        : "i"(BOX_ERR_FAULT)
    );
}

//// __box_abort glue ////

__attribute__((noreturn))
void __box_abort(int err) {
    // if there's no other course of action, we spin
    while (1) {}
}

__attribute__((used, noreturn))
void __wrap_abort(void) {
    __box_abort(-1);
}

#ifdef __GNUC__
__attribute__((noreturn))
void __assert_func(const char *file, int line,
        const char *func, const char *expr) {
    printf("%s:%d: assertion \"%s\" failed\n", file, line, expr);
    __box_abort(-1);
}

__attribute__((noreturn))
void _exit(int returncode) {
    __box_abort(-returncode);
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
        const char *np = strchr(p, '%');
        size_t skip = np ? np - p : strlen(p);

        if (skip > 0) {
            ssize_t nres = write(ctx, p, skip);
            if (nres < 0) {
                return nres;
            }
            res += nres;
        }

        // hit end of string?
        if (!np) {
            return res;
        }

        // format parser
        p = np;
        bool zero_justify = false;
        bool left_justify = false;
        bool precision_mode = false;
        size_t width = 0;
        size_t precision = 0;

        char mode = 'c';
        uint32_t value = 0;
        size_t size = 0;

        for (;; np++) {
            if (np[1] >= '0' && np[1] <= '9') {
                // precision/width
                if (precision_mode) {
                    precision = precision*10 + (np[1]-'0');
                } else if (np[1] > '0' || width > 0) {
                    width = width*10 + (np[1]-'0');
                } else {
                    zero_justify = true;
                }

            } else if (np[1] == '*') {
                // dynamic precision/width
                if (precision_mode) {
                    precision = va_arg(args, size_t);
                } else {
                    width = va_arg(args, size_t);
                }

            } else if (np[1] == '.') {
                // switch mode
                precision_mode = true;

            } else if (np[1] == '-') {
                // left-justify
                left_justify = true;

            } else if (np[1] == '%') {
                // single '%'
                mode = 'c';
                value = '%';
                size = 1;
                break;

            } else if (np[1] == 'c') {
                // char
                mode = 'c';
                value = va_arg(args, int);
                size = 1;
                break;

            } else if (np[1] == 's') {
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

            } else if (np[1] == 'd' || np[1] == 'i') {
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

            } else if (np[1] == 'u') {
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

            } else if (np[1] >= ' ' && np[1] <= '?') {
                // unknown modifier? skip

            } else {
                // hex or unknown character, terminate

                // make it prettier for pointers
                if (!(np[1] == 'x' || np[1] == 'X')) {
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
        p = np+2;

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
    // TODO hm, not const?
    return __box_write((int32_t)ctx, (void *)buf, size);
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
    // do nothing currently
    return 0;
}

#ifdef __GNUC__
int _write(int handle, char *buffer, int size) {
    return __box_write(handle, (uint8_t*)buffer, size);
}
#endif

//// ISR Vector definitions ////

extern void main(void);

// Reset Handler
__attribute__((naked, noreturn))
void __box_reset_handler(void) {
    // zero bss
    extern uint32_t __bss_start;
    extern uint32_t __bss_end;
    for (uint32_t *d = &__bss_start; d < &__bss_end; d++) {
        *d = 0;
    }

    // load data
    extern uint32_t __data_init_start;
    extern uint32_t __data_start;
    extern uint32_t __data_end;
    const uint32_t *s = &__data_init_start;
    for (uint32_t *d = &__data_start; d < &__data_end; d++) {
        *d = *s++;
    }

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

__attribute__((naked, noreturn))
void __box_default_handler(void) {
    while (1) {}
}

void __box_memmanage_handler(void);
void __box_busfault_handler(void);
void __box_usagefault_handler(void);

extern uint32_t __stack_end;

//// ISR Vector ////

__attribute__((used, section(".isr_vector")))
const uint32_t __isr_vector[256] = {
    (uint32_t)&__stack_end,
    (uint32_t)&__box_reset_handler,
    // Exception handlers
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_memmanage_handler,
    (uint32_t)__box_busfault_handler,
    (uint32_t)__box_usagefault_handler,
    (uint32_t)0,
    (uint32_t)0,
    (uint32_t)0,
    (uint32_t)0,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    (uint32_t)0,
    (uint32_t)__box_default_handler,
    (uint32_t)__box_default_handler,
    // External IRQ handlers
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

//// box1 init ////

int __box_box1_init(void) {
    int32_t err = __box_mpu_init();
    if (err) {
        return err;
    }

    // prepare box's stack
    // must use PSP, otherwise boxes could overflow ISR stack
    __box_box1_state.lr = 0xfffffffd; // TODO determine fp?
    __box_box1_state.sp = (uint32_t*)__box_box1_jumptable[0];

    // call box's init
    extern int __box_box1_rawinit(void);
    return __box_box1_rawinit();
}

//// box2 init ////

int __box_box2_init(void) {
    int32_t err = __box_mpu_init();
    if (err) {
        return err;
    }

    // prepare box's stack
    // must use PSP, otherwise boxes could overflow ISR stack
    __box_box2_state.lr = 0xfffffffd; // TODO determine fp?
    __box_box2_state.sp = (uint32_t*)__box_box2_jumptable[0];

    // call box's init
    extern int __box_box2_rawinit(void);
    return __box_box2_rawinit();
}

//// box3 init ////

int __box_box3_init(void) {
    int32_t err = __box_mpu_init();
    if (err) {
        return err;
    }

    // prepare box's stack
    // must use PSP, otherwise boxes could overflow ISR stack
    __box_box3_state.lr = 0xfffffffd; // TODO determine fp?
    __box_box3_state.sp = (uint32_t*)__box_box3_jumptable[0];

    // call box's init
    extern int __box_box3_rawinit(void);
    return __box_box3_rawinit();
}

