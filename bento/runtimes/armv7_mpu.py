
from .. import runtimes
from ..box import Fn
import itertools as it
import math

# utility functions in C
BOX_INIT = """
static void __box_%(box)s_init() {
    // zero bss
    extern uint32_t __bss;
    extern uint32_t __bss_end;
    for (uint32_t *d = &__bss; d < &__bss_end; d++) {
        *d = 0;
    }

    // load data
    extern uint32_t __data_init;
    extern uint32_t __data;
    extern uint32_t __data_end;
    const uint32_t *s = &__data_init;
    for (uint32_t *d = &__data; d < &__data_end; d++) {
        *d = *s++;
    }
}
"""

BOX_WRITE = """
//__attribute__((alias("__box_write")))
int _write(int handle, char *buffer, int size) {
    extern int __box_write(int handle, char *buffer, int size);
    // TODO hmm, why can't this alias?
    return __box_write(handle, buffer, size);
}
"""

MPU_REGISTERS = """
#define SHCSR    ((volatile uint32_t*)0xe000ed24)
#define MPU_TYPE ((volatile uint32_t*)0xe000ed90)
#define MPU_CTRL ((volatile uint32_t*)0xe000ed94)
#define MPU_RBAR ((volatile uint32_t*)0xe000ed9c)
#define MPU_RASR ((volatile uint32_t*)0xe000eda0)
"""

BOX_MPU_REGIONS = """
struct __box_mpuregions {
    uint32_t count;
    uint32_t regions[][2];
};
"""

BOX_SYS_DISPATCH = """
uint64_t __box_callsetup(uint32_t lr, uint32_t *sp,
        uint32_t op, uint32_t *fp) {
    // save lr + sp
    const uint32_t *jumptable = __box_jumptables[__box_active];
    uint32_t *state = (uint32_t*)jumptable[0];
    sp[0] = (uint32_t)fp;
    sp[1] = state[-2];
    sp[2] = state[-1];
    state[-2] = (uint32_t)lr;
    state[-1] = (uint32_t)sp;

    // TODO table walk
    uint32_t path = (op & %(callmask)#x)/2;
    uint32_t retbox = __box_active;
    __box_active = path %% __BOX_COUNT;
    const uint32_t *targetjumptable = __box_jumptables[__box_active];
    uint32_t *targetstate = (uint32_t*)targetjumptable[0];
    uint32_t targetlr = targetstate[-2];
    uint32_t *targetsp = (uint32_t*)targetstate[-1];
    // don't allow returns while executing
    targetstate[-2] = 0; 
    // need sp to fixup instruction aborts
    targetstate[-1] = (uint32_t)targetsp;
    uint32_t targetpc = targetjumptable[path / __BOX_COUNT + 1];

    // select MPU regions
    *MPU_CTRL = 0;
    const struct __box_mpuregions *regions = __box_mpuregions[__box_active];
    uint32_t count = regions->count;
    // TODO check restrictions
    for (int i = 0; i < %(mpuregions)d; i++) {
        if (i < count) {
            *MPU_RBAR = (~0x1f & regions->regions[i][0]) | 0x10 | (i+1);
            *MPU_RASR = regions->regions[i][1];
        } else {
            *MPU_RBAR = 0x10 | (i+1);
            *MPU_RASR = 0;
        }
    }
    *MPU_CTRL = 5;

    // enable control?
    uint32_t control;
    __asm__ volatile ("mrs %%0, control" : "=r"(control));
    control = (~1 & control) | (__box_active != 0 ? 1 : 0);
    __asm__ volatile ("msr control, %%0 \\n\\t isb" :: "r"(control) : "memory");

    // setup new call frame
    targetsp -= 8;
    targetsp[0] = fp[0];        // r0 = arg0
    targetsp[1] = fp[1];        // r1 = arg1
    targetsp[2] = fp[2];        // r2 = arg2
    targetsp[3] = fp[3];        // r3 = arg3
    targetsp[4] = fp[4];        // r12 = r12
    targetsp[5] = %(retprefix)#010x + retbox*2 + 1; // lr = __box_ret
    targetsp[6] = targetpc;     // pc = targetpc
    targetsp[7] = fp[7];        // psr = psr

    return ((uint64_t)targetlr) | ((uint64_t)(uint32_t)targetsp << 32);
}

__attribute__((naked, noreturn))
void __box_call(uint32_t lr, uint32_t *sp, uint32_t op) {
    __asm__ volatile (
        // keep track of args
        "mov r3, r1 \\n\\t"
        // save core registers
        "stmdb r1!, {r4-r11} \\n\\t"
        // save fp registers?
        "tst r0, #0x10 \\n\\t"
        "it eq \\n\\t"
        "vstmdbeq r1!, {s16-s31} \\n\\t"
        // make space to save state
        "sub r1, r1, #3*4 \\n\\t"
        // sp == msp?
        "tst r0, #0x4 \\n\\t"
        "it eq \\n\\t"
        "moveq sp, r1 \\n\\t"
        // ah! reserve a frame in case we're calling this
        // interrupts stack from another stack
        "sub sp, sp, #8*4 \\n\\t"
        // call into c now that we have stack control
        "bl __box_callsetup \\n\\t"
        // update new sp
        "tst r0, #0x4 \\n\\t"
        "itee eq \\n\\t"
        "msreq msp, r1 \\n\\t"
        "msrne psp, r1 \\n\\t"
        // drop reserved frame?
        "subne sp, sp, #8*4 \\n\\t"
        // return to call
        "bx r0 \\n\\t"
    );
}

uint64_t __box_retsetup(uint32_t lr, uint32_t *sp,
        uint32_t op, uint32_t *fp) {
    // save lr + sp
    const uint32_t *jumptable = __box_jumptables[__box_active];
    uint32_t *state = (uint32_t*)jumptable[0];
    // drop exception frame and fixup instruction aborts
    sp = (uint32_t*)state[-1];
    state[-2] = (uint32_t)lr;
    state[-1] = (uint32_t)sp;

    // TODO table walk
    uint32_t path = (op & %(retmask)#x)/2;
    __box_active = path %% __BOX_COUNT;
    const uint32_t *targetjumptable = __box_jumptables[__box_active];
    uint32_t *targetstate = (uint32_t*)targetjumptable[0];
    uint32_t targetlr = targetstate[-2];
    uint32_t *targetsp = (uint32_t*)targetstate[-1];
    uint32_t *targetfp = (uint32_t*)targetsp[0];
    targetstate[-2] = targetsp[1];
    targetstate[-1] = targetsp[2];

    // select MPU regions
    *MPU_CTRL = 0;
    const struct __box_mpuregions *regions = __box_mpuregions[__box_active];
    uint32_t count = regions->count;
    // TODO check restrictions
    for (int i = 0; i < %(mpuregions)d; i++) {
        if (i < count) {
            *MPU_RBAR = (~0x1f & regions->regions[i][0]) | 0x10 | (i+1);
            *MPU_RASR = regions->regions[i][1];
        } else {
            *MPU_RBAR = 0x10 | (i+1);
            *MPU_RASR = 0;
        }
    }
    *MPU_CTRL = 5;

    // enable control?
    uint32_t control;
    __asm__ volatile ("mrs %%0, control" : "=r"(control));
    control = (~1 & control) | (__box_active != 0 ? 1 : 0);
    __asm__ volatile ("msr control, %%0 \\n\\t isb" :: "r"(control) : "memory");

    // copy return frame
    targetfp[0] = fp[0];       // r0 = arg0
    targetfp[1] = fp[1];       // r1 = arg1
    targetfp[2] = fp[2];       // r2 = arg2
    targetfp[3] = fp[3];       // r3 = arg3
    targetfp[6] = targetfp[5]; // pc = lr

    return ((uint64_t)targetlr) | ((uint64_t)(uint32_t)targetsp << 32);
}

__attribute__((naked, noreturn))
void __box_ret(uint32_t lr, uint32_t *sp, uint32_t op) {
    __asm__ volatile (
        // keep track of rets
        "mov r3, r1 \\n\\t"
        // call into c new that we have stack control
        "bl __box_retsetup \\n\\t"
        // drop saved state
        "add r1, r1, #3*4 \\n\\t"
        // restore fp registers?
        "tst r0, #0x10 \\n\\t"
        "it eq \\n\\t"
        "vldmiaeq r1!, {s16-s31} \\n\\t"
        // restore core registers
        "ldmia r1!, {r4-r11} \\n\\t"
        // update sp
        "tst r0, #0x4 \\n\\t"
        "ite eq \\n\\t"
        "msreq msp, r1 \\n\\t"
        "msrne psp, r1 \\n\\t"
        // return
        "bx r0 \\n\\t"
    );
}

void __box_halt(uint32_t lr, uint32_t *sp, uint32_t op) {
    printf("Intercepted fault\\n");
    printf("CFSR 0x%%08x\\n", *(volatile uint32_t*)0xe000ed28);
    printf("MMFAR 0x%%08x\\n", *(volatile uint32_t*)0xe000ed34);
    printf("isr lr 0x%%08x\\n", lr);
    uint32_t pc = sp[6];
    printf("pc 0x%%08x\\n", pc);
    printf("sp 0x%%08x\\n", sp);
    printf("op 0x%%08x\\n", op);
    printf("MPU regions:\\n");
    for (int i = 0; i < %(mpuregions)d+1; i++) {
        *((volatile uint32_t*)0xe000ed98) = i;
        printf("MPU_RBAR 0x%%08x\\n", *(volatile uint32_t*)0xe000ed9c);
        printf("MPU_RASR 0x%%08x\\n", *(volatile uint32_t*)0xe000eda0);
    }
    while (1) {}
}

__attribute__((naked))
void MemManage_Handler(void) {
    __asm__ volatile (
        // get lr
        "mov r0, lr \\n\\t"
        "tst r0, #0x4 \\n\\t"
        // get sp
        "ite eq \\n\\t"
        "mrseq r1, msp \\n\\t"
        "mrsne r1, psp \\n\\t"
        // get pc
        "ldr r2, [r1, #6*4] \\n\\t"
        // call?
        "ldr r3, =#%(callprefix)#010x \\n\\t"
        "sub r3, r2, r3 \\n\\t"
        "lsrs r3, r3, #16 \\n\\t"
        "beq __box_call \\n\\t"
        // ret?
        "ldr r3, =#%(retprefix)#010x \\n\\t"
        "sub r3, r2, r3 \\n\\t"
        "lsrs r3, r3, #16 \\n\\t"
        "beq __box_ret \\n\\t" // TODO box ret
        // fallback to fault handler (call 1)
        // TODO this
        "b __box_halt \\n\\t"
    );
}
"""

BOX_SYS_INIT = """
extern int32_t __box_%(box)s_rawinit(void);
int32_t __box_%(box)s_init(void) {
    // make sure MPU is initialized
    if (!(*MPU_CTRL & 0x1)) {
        // do we have an MPU?
        assert(*MPU_TYPE);
        // enable MemManage exception
        *SHCSR = *SHCSR | 0x00010000;
        // setup call region
        *MPU_RBAR = %(callprefix)#010x | 0x10;
        // disallow execution
        //*MPU_RASR = 0x10230021;
        *MPU_RASR = 0x10000001 | ((%(callregionlog2)d-1) << 1);
        // enable the MPU
        *MPU_CTRL = 5;
    }

    // prepare box's stack
    uint32_t *state = (uint32_t*)__box_%(box)s_jumptable[0];
    // must use PSP, otherwise boxes could overflow ISR stack
    state[-2] = 0xfffffffd; // TODO determine fp?
    state[-1] = (uint32_t)(state - 2);

    // call box's init
    return __box_%(box)s_rawinit();
}
"""

@runtimes.runtime
class ARMv7MPURuntime(runtimes.Runtime):
    """
    A bento-box runtime that uses a v7 MPU to provide memory isolation
    between boxes.
    """
    __argname__ = "armv7_mpu"
    __arghelp__ = __doc__

    def __init__(self):
        super().__init__()
        self.ids = {}

    def box(self, box):
        super().box(box)
        # TODO provide this automatically?
        # we're running this multiple times, which technically works...
        parent = box.getparent()

        self.call_region = parent.mpu_call_region or Region(
            '0x1e000000-0x1fffffff')
        assert math.log2(self.call_region.size) % 1 == 0, (
            "MPU call region is not aligned to a power-of-two %s"
                % self.call_region)
        assert self.call_region.addr % self.call_region.size == 0, (
            "MPU call region is not aligned to size %s"
                % self.call_region)
        # TODO inject these into output config?
        self.callprefix = self.call_region.addr
        self.callmask = (self.call_region.size//2)-1
        self.retprefix = self.call_region.addr + self.call_region.size//2
        self.retmask = (self.call_region.size//2)-1
        self.callregionaddr = self.call_region.addr
        self.callregionsize = self.call_region.size
        self.callregionlog2 = int(math.log2(self.call_region.size))
        self.mpuregions = 4

        for box in parent.boxes:
            if box.runtime.name == self.__argname__:
                for memory in box.memories:
                    assert math.log2(memory.size) % 1 == 0, (
                        "Memory region %s not aligned to a power-of-two"
                            % memory.name)
                    assert memory.addr % memory.size == 0, (
                        "Memory region %s not aligned to its size"
                            % memory.name)
                    assert memory.size >= 32, (
                        "Memory region %s too small (< 32 bytes)"
                            % memory.name)

        self.ids = {}

        for j, export in enumerate(it.chain(
                ['__box_write'],
                (export.name for export in parent.exports))):
            self.ids[export] = j*(len(parent.boxes)+1)

        for i, box in enumerate(parent.boxes):
            # TODO make unique name?
            self.ids['box ' + box.name] = i+1
            for j, export in enumerate(it.chain(
                    ['__box_%s_rawinit' % box.name],
                    (export.name for export in box.exports))):
                self.ids[export] = j*(len(parent.boxes)+1) + i+1

    def build_common_c(self, output, box):
        output.pushattrs(
            callprefix=self.callprefix,
            callmask=self.callmask,
            retprefix=self.retprefix,
            retmask=self.retmask,
            callregionaddr=self.callregionaddr,
            callregionsize=self.callregionsize, 
            callregionlog2=self.callregionlog2,
            mpuregions=self.mpuregions)
        output.decls.append('//// jumptable declarations ////')

        outf = output.decls.append()
        outf.writef('struct %(box)s_exportjumptable {\n')
        with outf.pushindent():
            # special entries for the sp and __box_init
            outf.writef('uint32_t *__box_%(box)s_stack_end;\n')
            outf.writef('void (*__box_%(box)s_init)(void);\n')
            for export in box.exports:
                outf.writef('%(fn)s;\n', fn=export.repr_c_ptr())
        outf.writef('};\n')

        outf = output.decls.append()
        outf.writef('struct %(box)s_importjumptable {\n')
        with outf.pushindent():
            # special entries for __box_write and __box_fault
            outf.writef('void (*__box_%(box)s_fault)(void);\n')
            outf.writef('int (*__box_%(box)s_write)('
                'int a, char* b, int c);\n')
                # TODO are these the correct types??
            for import_ in box.imports:
                outf.writef('%(fn)s;\n', fn=import_.repr_c_ptr())
        outf.writef('};\n')

    def build_parent_prologue_c(self, output, sys):
        output.pushattrs(
            callprefix=self.callprefix,
            callmask=self.callmask,
            retprefix=self.retprefix,
            retmask=self.retmask,
            callregionaddr=self.callregionaddr,
            callregionsize=self.callregionsize, 
            callregionlog2=self.callregionlog2,
            mpuregions=self.mpuregions)
        output.decls.append('//// jumptable implementation ////')
        output.decls.append(
            'extern int _write(int handle, char *buffer, int size);',
            doc='GCC stdlib hook')

        output.decls.append(MPU_REGISTERS)

        outf = output.decls.append(doc='System state')
        outf.writef('uint32_t __box_active = 0;\n')
        outf.writef('uint32_t __box_sys_state[2];\n')

        # jumptables
        outf = output.decls.append()
        outf.writef('const uint32_t __box_sys_jumptable[] = {\n')
        with outf.pushindent():
            outf.writef('(uint32_t)(&__box_sys_state + 1),\n')
            outf.writef('(uint32_t)&_write,\n')
            for export in sys.exports:
                outf.writef('(uint32_t)%(export)s,\n', export=export.name)
        outf.write('};')

        outf = output.decls.append()
        for box in sys.boxes:
            if box.runtime.name == self.__argname__:
                outf.writef(
                    'extern const uint32_t __box_%(box)s_jumptable[];\n',
                    box=box.name)

        outf = output.decls.append()
        outf.writef('#define __BOX_COUNT %(boxcount)d\n',
            boxcount=sum(1 for box in sys.boxes
                if box.runtime.name == self.__argname__) + 1)
        outf.writef('const uint32_t *const '
            '__box_jumptables[__BOX_COUNT] = {\n');
        with outf.pushindent():
            outf.writef('__box_sys_jumptable,\n')
            for box in sys.boxes:
                if box.runtime.name == self.__argname__:
                    outf.writef('__box_%(box)s_jumptable,\n',
                        box=box.name)
        outf.writef('};');

        # mpu regions
        output.decls.append(BOX_MPU_REGIONS)

        outf = output.decls.append()
        outf.writef('const struct __box_mpuregions __box_sys_mpuregions = {\n')
        with outf.pushindent():
            outf.writef('.count = 0,\n')
            outf.writef('.regions = {},\n')
        outf.writef('};\n')

        for box in sys.boxes:
            if box.runtime.name == self.__argname__:
                outf = output.decls.append(box=box.name)
                outf.writef('const struct __box_mpuregions '
                    '__box_%(box)s_mpuregions = {\n')
                with outf.pushindent():
                    outf.writef('.count = %(count)d,\n',
                        count=len(box.memories))
                    outf.writef('.regions = {\n')
                    with outf.pushindent():
                        for memory in box.memories:
                            outf.writef('{%(rbar)#010x, %(rasr)#010x},\n',
                                rbar=memory.addr,
                                rasr= (0x10000000
                                        if 'x' not in memory.mode else
                                        0x00000000)
                                    | (0x03000000
                                        if set('rw').issubset(memory.mode) else
                                        0x02000000
                                        if 'r' in memory.mode else
                                        0x00000000)
                                    | 0 #(0x00080000)
                                    | ((int(math.log2(memory.size))-1) << 1)
                                    | 1)
                    outf.writef('},\n')
                outf.writef('};')

        outf = output.decls.append()
        outf.writef('const struct __box_mpuregions *const '
            '__box_mpuregions[__BOX_COUNT] = {\n')
        with outf.pushindent():
            outf.writef('&__box_sys_mpuregions,\n')
            for box in sys.boxes:
                if box.runtime.name == self.__argname__:
                    outf.writef('&__box_%(box)s_mpuregions,\n', box=box.name)
        outf.writef('};\n')

        # the rest of the dispatch logic
        output.includes.append('<assert.h>') # TODO need this?
        output.decls.append(BOX_SYS_DISPATCH)

    def build_parent_c(self, output, sys, box):
        output.pushattrs(
            callprefix=self.callprefix,
            callmask=self.callmask,
            retprefix=self.retprefix,
            retmask=self.retmask,
            callregionaddr=self.callregionaddr,
            callregionsize=self.callregionsize, 
            callregionlog2=self.callregionlog2,
            mpuregions=self.mpuregions)
        output.decls.append(BOX_SYS_INIT)

    def build_c(self, output, box):
        output.pushattrs(
            callprefix=self.callprefix,
            callmask=self.callmask,
            retprefix=self.retprefix,
            retmask=self.retmask,
            callregionaddr=self.callregionaddr,
            callregionsize=self.callregionsize, 
            callregionlog2=self.callregionlog2,
            mpuregions=self.mpuregions)
        self.build_common_c(output, box)

        output.decls.append('//// jumptable implementation ////')
        output.decls.append(BOX_INIT)
        output.decls.append(BOX_WRITE)

        output.decls.append('extern uint32_t __stack_end;')
        outf = output.decls.append(doc='box-side jumptable')
        outf.writef('__attribute__((section(".jumptable")))\n')
        outf.writef('__attribute__((used))\n')
        outf.writef('const struct %(box)s_exportjumptable '
            '__box_%(box)s_jumptable = {\n')
        with outf.pushindent():
            # special entries for the sp and __box_init
            outf.writef('&__stack_end,\n')
            outf.writef('__box_%(box)s_init,\n')
            for export in box.exports:
                outf.writef('%(export)s,\n', export=export.name)
        outf.writef('};\n')

    def build_parent_partial_ld(self, output, sys, box):
        output.pushattrs(
            callprefix=self.callprefix,
            callmask=self.callmask,
            retprefix=self.retprefix,
            retmask=self.retmask,
            callregionaddr=self.callregionaddr,
            callregionsize=self.callregionsize, 
            callregionlog2=self.callregionlog2,
            mpuregions=self.mpuregions)
        # create box calls for imports
        output.decls.append('/* box calls */')
        for import_ in it.chain(
                # TODO rename this
                ['__box_%(box)s_rawinit'],
                (import_.name for import_ in box.exports)):
            output.decls.append(
                '%(import_)-24s = %(callprefix)#010x + %(id)d*2 + 1;',
                import_=import_,
                id=self.ids[import_ % dict(box=box.name)])
        output.decls.append()

        memory, _, _ = box.consume('rx', box.jumptable.size)
        outf = output.sections.insert(0,
            box_memory=memory.name,
            section='.box.%(box)s.%(box_memory)s',
            memory='box_%(box)s_%(box_memory)s')
        outf.writef('. = ORIGIN(%(MEMORY)s);\n')
        outf.writef('__box_%(box)s_jumptable = .;')
        super().build_parent_partial_ld(output, sys, box)

    def build_parent_ld(self, output, sys, box):
        output.pushattrs(
            callprefix=self.callprefix,
            callmask=self.callmask,
            retprefix=self.retprefix,
            retmask=self.retmask,
            callregionaddr=self.callregionaddr,
            callregionsize=self.callregionsize, 
            callregionlog2=self.callregionlog2,
            mpuregions=self.mpuregions)
        return self.build_parent_partial_ld(output, sys, box)

    def build_partial_ld(self, output, box):
        output.pushattrs(
            callprefix=self.callprefix,
            callmask=self.callmask,
            retprefix=self.retprefix,
            retmask=self.retmask,
            callregionaddr=self.callregionaddr,
            callregionsize=self.callregionsize, 
            callregionlog2=self.callregionlog2,
            mpuregions=self.mpuregions)
        # create box calls for imports
        output.decls.append('/* box calls */')
        for i, import_ in enumerate(it.chain(
                ['__box_write'],
                (import_.name for import_ in box.imports))):
            output.decls.append(
                '%(import_)-24s = %(callprefix)#010x + %(id)d*2 + 1;',
                import_=import_,
                id=self.ids[import_])
        output.decls.append()

    def build_ld(self, output, box):
        output.pushattrs(
            callprefix=self.callprefix,
            callmask=self.callmask,
            retprefix=self.retprefix,
            retmask=self.retmask,
            callregionaddr=self.callregionaddr,
            callregionsize=self.callregionsize, 
            callregionlog2=self.callregionlog2,
            mpuregions=self.mpuregions)
        self.build_partial_ld(output, box)
        
        # TODO handle this in ldscript class? ldscript.consume?
        # TODO make jumptable come before ldscript declarations.
        # Need box method?
        # TODO what... this just doesn't work...
#        memory = box.bestmemory('rx', box.jumptable.size,
#            consumed=output.consumed)
        memory, _, _ = box.consume('rx', section=box.jumptable)
        #print(output.consumed)
        # TODO hm
        #output.consumed[memory.name] += box.jumptable.size
        outf = output.sections.insert(0,
            section='.jumptable',
            memory=memory.name)
            # TODO move these?
            # TODO shortcut for capitalized?
#            prefixed_section='%(section_prefix)s%(section)s',
#            prefixed_memory='%(memory_prefix)s%(memory)s',
#            prefixed_symbol='%(symbol_prefix)s%(symbol)s', symbol='')
        outf.writef('%(section)s : {\n')
        with outf.pushindent():
            outf.writef('. = ALIGN(%(align)d);\n')
            outf.writef('__jumptable = .;\n')
            outf.writef('KEEP(*(.jumptable))\n')
            outf.writef('. = ALIGN(%(align)d);\n')
            outf.writef('__jumptable_end = .;\n')
        outf.writef('} > %(MEMORY)s')

        super().build_ld(output, box)

