import re
import collections as co
import itertools as it
import os
from . import argstuff
from .argstuff import ArgumentParser


class Section:
    """
    Description of the SECTION section. Note that the SECTION section
    may not be emitted depending on specified outputs and runtimes.
    """
    __argname__ = "section"
    __arghelp__ = __doc__
    @classmethod
    def __argparse__(cls, parser, name=None, help=None):
        name = name or cls.__argname__
        help = (help or cls.__arghelp__).replace('SECTION', name)
        parser.add_argument("size", pred=cls.parsesection,
            metavar=name, help=help)
        parser.add_argument("--size", type=lambda x: int(x, 0),
            help="Minimum size of the %s. Defaults to 0." % name)
        parser.add_argument("--align", type=lambda x: int(x, 0),
            help="Minimum alignment of the %s. Optional." % name)
        parser.add_argument("--memory",
            help="Explicitly specify the memory to allocate the %s." % name)

    @staticmethod
    def parsesection(s):
        addrpattern = r'(?:0[a-z])?[0-9a-fA-F]+'
        bufferpattern = (r'\s*'
            '(%(addr)s)?\s*'
            '(%(addr)s)?\s*'
            '(?:bytes)?\s*' % dict(addr=addrpattern))

        m = re.match('^%s$' % bufferpattern, s)
        if not m:
            raise ValueError("Invalid %s description %r" % (
                cls.__argname__, s))

        size = int(m.group(1), 0) if m.group(1) else None
        if m.group(2) and int(m.group(2), 0) != size:
            raise ValueError("Range %#x does not match size %#x" % (
                size, int(m.group(4), 0)))

        return size

    def __init__(self, size=None, align=None, memory=None):
        self.size = (Section.parsesection(size)
            if isinstance(size, str) else
            size or 0)
        self.align = align
        self.memory = memory

        if self.align:
            assert self.size % self.align == 0, ("Section size %#010x "
                "not aligned to section alignment %#010x" % (
                    self.size, self.align))

    def __str__(self):
        return "%(size)#010x %(size)s bytes" % dict(size=self.size)

    def __bool__(self):
        return self.size != 0

class Region:
    """
    Region of addresses to use for REGION.
    """
    __argname__ = "region"
    __arghelp__ = __doc__
    @classmethod
    def __argparse__(cls, parser, name=None, help=None):
        name = name or cls.__argname__
        help = (help or cls.__arghelp__).replace('REGION', name)
        parser.add_argument("region", pred=cls.parseregion,
            help=cls.__arghelp__)
        parser.add_argument("--addr", type=lambda x: int(x, 0),
            help="Starting address of region. Note that addr may be "
                "undefined if the exact location does not matter.")
        parser.add_argument("--size", type=lambda x: int(x, 0),
            help="Size of the region in bytes.")

    @staticmethod
    def parseregion(s):
        addrpattern = r'(?:0[a-z])?[0-9a-fA-F]+'
        mempattern = (r'\s*'
            '(?:(%(addr)s)\s*-)?\s*'
            '(%(addr)s)?\s*'
            '(%(addr)s)?\s*'
            '(?:bytes)?\s*' % dict(addr=addrpattern))

        m = re.match('^%s$' % mempattern, s)
        if not m:
            raise ValueError("Invalid region description %r" % s)

        addr = m.group(1) and int(m.group(1), 0)
        size = m.group(2) and int(m.group(2), 0) - (
            addr - 1 if addr is not None else 0)
        if m.group(3) and int(m.group(3), 0) != size:
            raise ValueError("Range %#x-%#x does not match size %#x" % (
                addr or 0, addr+size-1, int(m.group(3), 0)))

        return addr, size

    def __init__(self, region=None, addr=None, size=None):
        region = Region.parseregion(region) if region else (None, None)
        self.addr = addr if addr is not None else region[0]
        self.size = size if size is not None else region[1] or 0
        assert self.size >= 0, "Invalid region size %#x?" % self.size

    def __str__(self):
        return "%(range)s %(size)d bytes" % dict(
            range='%#010x-%#010x' % (self.addr, self.addr+self.size-1)
                if self.addr is not None else
                '%#010x' % self.size,
            size=self.size)

    def __bool__(self):
        return bool(self.size)

    def __lt__(self, other):
        return self.addr < other.addr

    def __contains__(self, other):
        if isinstance(other, Region):
            return (other.addr >= self.addr and
                other.addr+other.size <= self.addr+self.size)
        else:
            return (other >= self.addr and
                other < self.addr + self.size)

    def __sub__(self, regions):
        if isinstance(regions, Region):
            regions = [regions]

        if self.size == 0:
            return []

        slices = [(self.addr, self.size)]
        for region in regions:
            nslices = []
            for addr, size in slices:
                if region.addr >= addr+size or region.addr+region.size <= addr:
                    nslices.append((addr, size))
                else:
                    if region.addr > addr:
                        nslices.append((addr, region.addr - addr))
                    if addr+size > region.addr+region.size:
                        nslices.append((region.addr+region.size,
                            addr+size - region.addr+region.size))
            slices = nslices

        return [Region(addr=addr, size=size) for addr, size in slices]

class Memory(Region):
    """
    Description of a memory region named MEMORY.
    """
    MODEFLAGS = ['r', 'w', 'x']

    __argname__ = "memory"
    __arghelp__ = __doc__
    @classmethod
    def __argparse__(cls, parser, **kwargs):
        parser.add_argument("memory", pred=cls.parsememory,
            help=cls.__arghelp__)
        parser.add_argument("--mode", pred=cls.parsemode,
            help="String of characters indicating how the underlying memory "
                "can be accessed. Can be a combination of %s." %
                ', '.join(map(repr, cls.MODEFLAGS)))
        parser.add_argument("--addr", type=lambda x: int(x, 0),
            help="Starting address of memory region. Note that addr may be "
                "undefined if the exact location does not matter.")
        parser.add_argument("--size", type=lambda x: int(x, 0),
            help="Size of memory region in bytes.")
        parser.add_argument("--align", type=lambda x: int(x, 0),
            help="Minimum alignment of the memory region.")

    @staticmethod
    def parsemode(s):
        if any(c not in Memory.MODEFLAGS for c in s):
            raise ValueError("invalid memory mode %r" % s)
        return set(s)

    @staticmethod
    def parsememory(s):
        addrpattern = r'(?:0[a-z])?[0-9a-fA-F]+'
        mempattern = (r'\s*'
            '([a-z-]+(?=\s|$))?\s*'
            '(?:(%(addr)s)\s*-)?\s*'
            '(%(addr)s)?\s*'
            '(%(addr)s)?\s*'
            '(?:bytes)?\s*' % dict(addr=addrpattern))

        m = re.match('^%s$' % mempattern, s)
        if not m:
            raise ValueError("Invalid memory description %r" % s)

        mode = m.group(1) and Memory.parsemode(m.group(1))
        addr = m.group(2) and int(m.group(2), 0)
        size = m.group(3) and int(m.group(3), 0) - (
            addr - 1 if addr is not None else 0)
        if m.group(4) and int(m.group(4), 0) != size:
            raise ValueError("Range %#x-%#x does not match size %#x" % (
                addr or 0, addr+size-1, int(m.group(4), 0)))

        return mode, addr, size

    def __init__(self, name, memory=None,
            mode=None, addr=None, size=None, align=None):
        self.name = name
        memory = Memory.parsememory(memory) if memory else (None, None, None)
        self.mode = Memory.parsemode(mode) if mode else memory[0] or set()
        self.align = align
        self.addr = addr if addr is not None else memory[1]
        self.size = size if size is not None else memory[2] or 0

        assert self.size >= 0, "Invalid memory size %#x?" % self.size
        if self.align and self.addr:
            assert self.addr % self.align == 0, ("Memory address %#010x "
                "not aligned to memory alignment %#010x" % (
                    self.addr, self.align))
        if self.align:
            assert self.size % self.align == 0, ("Memory size %#010x "
                "not aligned to memory alignment %#010x" % (
                    self.size, self.align))

        # modified size after memory is consumed
        self._addr = self.addr
        self._size = self.size

    def __str__(self):
        return "%(mode)s %(range)s %(size)d bytes" % dict(
            mode=''.join(c if c in self.mode else '-'
                for c in Memory.MODEFLAGS),
            range='%#010x-%#010x' % (self.addr, self.addr+self.size-1)
                if self.addr is not None else
                '%#010x' % self.size,
            size=self.size)

    def __lt__(self, other):
        return (self.addr, self.name) < (other.addr, other.name)

    def used(self):
        return self.size - self._size

    def unused(self):
        return self._size

    def consume(self, size=None, align=None, section=None, reverse=False):
        if section is not None:
            size = section.size
            align = section.align
        assert align is None, "TODO" # TODO
        assert size <= self._size, ("Not enough memory in %s "
            "for size=%#010x" % (self.name, size))

        if not reverse:
            self._addr += size
            self._size -= size
            return Memory(self.name, mode=self.mode,
                addr=self._addr, size=size, align=None)
        else:
            self._size -= size
            return Memory(self.name, mode=self.mode,
                addr=self._addr + self._size, size=size, align=None)

    def iscompatible(self, mode='rwx', size=None, align=None,
            section=None, memory=None):
        if section is not None:
            size = section.size
            align = section.align
            memory = section.memory
        if isinstance(memory, Memory):
            memory = memory.name
        assert align is None, "TODO" # TODO
        return (
            self._size != 0 and
            set(mode).issubset(self.mode) and
            (memory is None or memory == self.name) and
            (size is None or size <= self._size))

    @staticmethod
    def bestkey(mode='rwx', size=None, align=None,
            section=None, memory=None, reverse=False):
        def key(self):
            return (
                len(self.mode - set(mode)),
                -self.addr if reverse else self.addr)
        return key

    def __sub__(self, regions):
        return [
            Memory(self.name, mode=self.mode, align=self.align,
                addr=region.addr, size=region.size)
            for region in super().__sub__(regions)]

class Type:
    """
    Type of function argument or return value.
    """
    PRIMITIVE_TYPES = [
        'u8', 'u16', 'u32', 'u64', 'usize',
        'i8', 'i16', 'i32', 'i64', 'isize',
        'err32', 'err64', 'errsize',
        'f32', 'f64']
    PRIMITIVE_ALIASES = [
        (r'\berr\b',            r'err32'),
        (r'\bu\b',              r'u32'),
        (r'\bi\b',              r'i32'),
        (r'\buint(\d+)_t\b',    r'u\1'),
        (r'\bint(\d+)_t\b',     r'i\1'),
        (r'\bsize_t\b',         r'usize'),
        (r'\bssize_t\b',        r'isize'),
        (r'\bfloat\b',          r'f32'),
        (r'\bdouble\b',         r'f64'),
        (r'\bvoid *\*',         r'u8*')]
    def __init__(self, type):
        for pattern, repl in it.chain([(r' +', r'')], Type.PRIMITIVE_ALIASES):
            type = re.sub(pattern, repl, type)

        m = re.match(r'^(%s)(\**)$' % '|'.join(Type.PRIMITIVE_TYPES), type)
        if not m:
            raise ValueError("Invalid type %r" % type)

        self._primitive = m.group(1)
        self._ptr = m.group(2)

        if len(self._ptr) > 1:
            raise ValueError(
                "Indirect pointers currently not supported in type %r" % type)

    def isptr(self):
        return bool(self._ptr)

    def iserr(self):
        return self._primitive.startswith('err')

    def __eq__(self, other):
        return ((self._primitive, self._ptr)
            == (other._primitive, other._ptr))

    def __str__(self, argname=None):
        if argname:
            return '%s %s%s' % (self._primitive, self._ptr, argname)
        else:
            return '%s%s' % (self._primitive, self._ptr)

    def repr_c(self, name=None):
        if self._primitive == 'u8' and self._ptr:
            primitive = 'void'
        elif self._primitive == 'err32':
            primitive = 'int'
        elif self._primitive == 'errsize':
            primitive = 'ssize_t'
        elif self._primitive.startswith('err'):
            primitive = 'int%s_t' % self._primitive[3:]
        elif self._primitive == 'usize':
            primitive = 'size_t'
        elif self._primitive == 'isize':
            primitive = 'ssize_t'
        elif self._primitive.startswith('u'):
            primitive = 'uint%s_t' % self._primitive[1:]
        elif self._primitive.startswith('i'):
            primitive = 'int%s_t' % self._primitive[1:]
        elif self._primitive == 'f32':
            primtive = 'float'
        elif self._primitive == 'f64':
            primtive = 'double'
        else:
            assert False, 'unknown type %r' % self._primitive

        if name:
            return '%s %s%s' % (primitive, self._ptr, name)
        else:
            return '%s%s' % (primitive, self._ptr)

class Link:
    """
    Simple tuple for connecting functions.
    """
    def __init__(self, export, import_):
        self.export = export
        self.import_ = import_

class Fn:
    """
    Function type.
    """
    @classmethod
    def __argparse__(cls, parser, **kwargs):
        parser.add_argument("type", pred=cls.parsetype,
            help=cls.__arghelp__)
        parser.add_argument("--type", pred=cls.parsetype,
            help="Type of the function.")
        parser.add_argument("--alias",
            help="Name used in the box for the function.")
        parser.add_argument("--doc",
            help="Documentation for the function.")

    @staticmethod
    def parsetype(s):
        namepattern = r'[a-zA-Z_][a-zA-Z_0-9]*'
        typepattern = r'%(name)s(?:\s*[\*\&])*' % dict(name=namepattern)
        # three arg permutations: tpye, name: type, type name
        argpattern = (r'(?:'
                r'(%(type)s)|'
                r'(%(name)s):\s*(%(type)s)|'
                r'(%(type)s)\s*(%(name)s)'
            r')' % dict(name=namepattern, type=typepattern))
        # functions with and without parens around args/rets
        fnpattern = (r'\s*fn\s*(?:'
                r'\(\s*((?:%(arg)s(?:\s*,\s*%(arg)s)*)?)\s*\)|'
                r'((?:%(arg)s(?:\s*,\s*%(arg)s)*)?)'
            r')\s*->\s*(?:'
                r'\(\s*((?:%(arg)s(?:\s*,\s*%(arg)s)*)?)\s*\)|'
                r'((?:%(arg)s(?:\s*,\s*%(arg)s)*)?)'
            r')\s*' % dict(arg=argpattern))

        m = re.match('^%s$' % fnpattern, s)
        if not m:
            raise ValueError("Invalid import/export type %r" % s)

        rawargs = (m.group(1) or m.group(12) or '').split(',')
        rawrets = (m.group(23) or m.group(34) or '').split(',') # 35?

        args = []
        argnames = []
        for arg in rawargs:
            if arg:
                m = re.match('^%s$' % argpattern, arg.strip())
                name = m.group(2) or m.group(5)
                type = (m.group(1) or m.group(3) or m.group(4)).replace(' ', '')
                args.append(type)
                argnames.append(name)
        if args == ['void']:
            args = []

        rets = []
        retnames = []
        for ret in rawrets:
            if ret:
                m = re.match('^%s$' % argpattern, ret.strip())
                name = m.group(2) or m.group(5)
                type = (m.group(1) or m.group(3) or m.group(4)).replace(' ', '')
                rets.append(type)
                retnames.append(name)
        if rets == ['void']:
            rets = []

        if len(rets) > 1:
            raise ValueError("Currently only 0 or 1 return values supported")

        return args, argnames, rets, retnames

    def __init__(self, name, type, alias=None, doc=None,
            source=None, weak=False):
        self.name = name
        self.targetname = name.split('.', 1)[0] if '.' in name else None
        self.linkname = name.split('.', 1)[-1]
        self.alias = alias or self.linkname
        self.doc = doc
        self.source = source
        self.weak = weak

        argtypes, argnames, rettypes, retnames = Fn.parsetype(type)
        self.args = [Type(arg) for arg in argtypes]
        self.argnames = [name or 'a%d' % i for i, name in enumerate(argnames)]
        self.rets = [Type(ret) for ret in rettypes]
        self.retnames = [name or 'a%d' % i for i, name in enumerate(retnames)]

    def __str__(self):
        return 'fn(%s) -> %s' % (
            ', '.join(map(str, self.args)),
            'void' if not self.rets else
            ', '.join(map(str, self.rets)))

    def __lt__(self, other):
        return self.name < other.name

    def repr_c(self, name=None):
        return "%(rets)s %(name)s(%(args)s)" % dict(
            name=name or self.alias,
            args='void' if len(self.args) == 0 else
                ', '.join(arg.repr_c(name) for arg, name in
                    zip(self.args, self.argnames)),
            rets='void' if len(self.rets) == 0 else
                ', '.join(ret.repr_c() for ret in self.rets))

    def repr_c_ptr(self, name=None):
        return self.repr_c(name='(*%s)' % (name or self.alias))

    def isfalible(self):
        return any(ret.iserr() for ret in self.rets)

    def iscompatible(self, other):
        return (self.args == other.args and self.rets == other.rets)

    def islinkable(self, other, exportbox=None, importbox=None):
        return (self.linkname == other.linkname and 
            (self.targetname is None or
                exportbox is None or
                self.targetname == exportbox) and
            (other.targetname is None or
                importbox is None or
                other.targetname == importbox))

class Import(Fn):
    """
    Description of an imported function for a box.
    """
    __argname__ = "import"
    __arghelp__ = __doc__
    def __init__(self, name, *args, **kwargs):
        super().__init__(name, *args, **kwargs)
        # these get populated later
        self.box = None
        self.link = None

class Export(Fn):
    """
    Description of an exported function for a box.
    """
    __argname__ = "export"
    __arghelp__ = __doc__
    def __init__(self, name, *args, **kwargs):
        super().__init__(name, *args, **kwargs)
        # these get populated later
        self.box = None
        self.links = []

class Box:
    """
    Recursive description of a given box named BOX.
    """
    __argname__ = "box"
    __arghelp__ = __doc__
    @classmethod
    def __argparse__(cls, parser, recursive=False, fake=False, **kwargs):
        parser.add_argument('--name',
            help='Name of the box.')
        parser.add_argument('--path',
            help='Working directory for the box. Defaults to the '
                'name of the box.')
        parser.add_argument('--recipe',
            help='Path to reciple.toml file for box-specific configuration.'
                'Defaults to <path>/recipe.toml.')

        from .runtimes import RUNTIMES
        runtimeparser = parser.add_nestedparser('--runtime')
        runtimeparser.add_argument("select",
            metavar='RUNTIME', choices=RUNTIMES,
            help='Runtime for the box. This can be one of the following '
                'runtimes: {%(choices)s}.')
        runtimeparser.add_argument("--select",
            metavar='RUNTIME', choices=RUNTIMES,
            help='Runtime for the box. This can be one of the following '
                'runtimes: {%(choices)s}.')
        for Runtime in RUNTIMES.values():
            runtimeparser.add_nestedparser(Runtime)

        from .outputs import OUTPUTS
        outputparser = parser.add_nestedparser('--output')
        for Output in OUTPUTS.values():
            outputparser.add_nestedparser(Output)

        parser.add_set(Memory)
        parser.add_nestedparser('--stack', Section)
        parser.add_nestedparser('--heap', Section)
        parser.add_nestedparser('--text', Section)
        parser.add_nestedparser('--data', Section)
        parser.add_nestedparser('--bss', Section)

        parser.add_set(Import)
        parser.add_set(Import, metavar='BOX.IMPORT', depth=2)
        parser.add_set(Export)
        parser.add_set(Export, metavar='BOX.EXPORT', depth=2)

    def __init__(self, name=None, parent=None, path=None, recipe=None,
            runtime=None, output=None, memory=None,
            stack=None, heap=None, text=None, data=None, bss=None,
            export=None, box=None, **kwargs):
        self.name = name or 'system'
        self.parent = parent
        self.path = path
        self.recipe = recipe

        from .runtimes import RUNTIMES
        selected = runtime.select or 'noop'
        self.runtime = RUNTIMES[selected](**getattr(
            runtime, selected, argstuff.Namespace()).__dict__)

        from .outputs import OUTPUTS
        self.outputs = sorted(
            OUTPUTS[name](os.path.join(self.path, outputargs.path),
                **{k: v for k, v in outputargs.__dict__.items()
                    if k != 'path'})
            for name, outputargs in output.__dict__.items()
            if outputargs.path)

        self.memories = sorted(
            Memory(name, **memargs.__dict__)
            for name, memargs in memory.items())
        self.memoryslices = self.memories

        self.stack = Section(**stack.__dict__)
        self.heap = Section(**heap.__dict__)
        self.text = Section(**text.__dict__)
        self.data = Section(**data.__dict__)
        self.bss = Section(**bss.__dict__)

        self.imports = sorted(
            Import(name, source=self, **importargs.__dict__)
            for name, importargs in it.chain.from_iterable(
                [('%s.%s' % (k, k2), v2) for k2, v2 in v.items()]
                if isinstance(v, dict) else
                [(k, v)]
                for k, v in kwargs['import'].items()))

        self.exports = sorted(
            Export(name, source=self, **exportargs.__dict__)
            for name, exportargs in it.chain.from_iterable(
                [('%s.%s' % (k, k2), v2) for k2, v2 in v.items()]
                if isinstance(v, dict) else
                [(k, v)]
                for k, v in export.items()))

        self.boxes = [] 

        # prepare build/box commands for prologue/epilogue handling
        self.box_prologues = co.OrderedDict()
        self.box_epilogues = co.OrderedDict()
        self.link_prologues = co.OrderedDict()
        self.link_epilogues = co.OrderedDict()
        self.build_prologues = co.OrderedDict()
        self.build_epilogues = co.OrderedDict()

    def __eq__(self, other):
        if isinstance(other, Box):
            return self.name == other.name
        else:
            return self.name == other

    def __lt__(self, other):
        return self.name < other.name

    def bestmemories(self, mode='rwx', size=None, align=None,
            section=None, memory=None, reverse=False):
        return sorted((
                m for m in self.memoryslices
                if m.iscompatible(mode=mode, size=size, align=align,
                    section=section, memory=memory)),
            key=Memory.bestkey(mode=mode, size=size, align=align,
                section=section, memory=memory, reverse=reverse))

    def bestmemory(self, mode='rwx', size=None, align=None,
            section=None, memory=None, reverse=False):
        compatible = self.bestmemories(mode=mode, size=size, align=align,
            section=section, memory=memory, reverse=reverse)
        return compatible[0] if compatible else None

    def consume(self, mode='rwx', size=None, align=None,
            section=None, memory=None, reverse=False):
        best = self.bestmemory(mode=mode, size=size, align=align,
            section=section, memory=memory, reverse=reverse)
        assert best, ("No memory found that satisfies "
            "mode=%s size=%#010x" % (mode,
                (section.size if section else size) or 0))
        return best.consume(size=size, align=align,
            section=section, reverse=reverse)

    def getroot(self):
        """
        Get the root box of the current tree.
        """
        root = self
        while root.parent:
            root = root.parent
        return root if root != self else None

    def getmuxer(self):
        """
        Returns the muxer box of the current runtime. This is defined to be
        nearest parent that is a different runtime. This is the point where
        tree muxing must be done.
        """
        muxer = self
        while muxer.parent and muxer.runtime == self.runtime:
            muxer = muxer.parent
        return muxer if muxer != self else None

    def getparent(self, n=1):
        """
        Returns a box's nth parent, if it has an nth parent.
        """
        parent = self
        for _ in range(n):
            if not self.parent:
                return None
            parent = self.parent
        return parent if parent != self else None

    def addimport(self, import_, *args, **kwargs):
        kwargs.setdefault('source', self)
        if not isinstance(import_, Export):
            import_ = Import(import_, *args, **kwargs)
        self.imports.append(import_)
        return import_

    def addexport(self, export, *args, **kwargs):
        kwargs.setdefault('source', self)
        if not isinstance(export, Export):
            export = Export(export, *args, **kwargs)
        self.exports.append(export)
        return export

    @staticmethod
    def _scan_argparse(parser):
        # add root box
        Box.__argparse__(parser)
        # add nested boxes
        parser.add_set(Box, glob=True, action='append',
            help='Recursive description of a given box named BOX that is '
                'contained in the current box.')
        parser.add_set('--super', cls=Box, glob=True, action='append',
            help='Recursive description of superboxes. These are boxes '
                'that contain the current box. Note, each box can only have '
                'one superbox, however that superbox can have other boxes.')
        # also add a shortcut for providing box defaults
        parser.add_nestedparser('--all', cls=Box, glob=True,
            help='Alias for all boxes. This allows you to specify config '
                'that all boxes inherit unless explicitly overwritten.')

    @staticmethod
    def scan(name=None, **args):
        """
        Build up tree of config information for boxes. This handles box
        creation and reading recipe.json files from the filesystem. Tree
        is rotated so superbox is returned, even if it's not located in the
        current directory.
        """
        # prepare parser
        parser = ArgumentParser(add_help=False)
        Box.scan.__argparse__(parser)

        def recurse(prefix=None, parent=None, child=None,
                parentallargs=argstuff.Namespace(), **args):
            # reparse
            args = parser.parse_dict(args, prefix=prefix)

            # load additional config from the filesystem
            path = args.path or '.'
            recipe = args.recipe or 'recipe.toml'
            try:
                nargs = parser.parse_toml(os.path.join(path, recipe),
                    prefix=prefix)
                args = argstuff.nsmerge(args, nargs)
            except FileNotFoundError:
                pass

            # apply all
            allargs = parser.parse_dict(args.all)
            allargs = argstuff.nsmerge(parentallargs, allargs)
            args = argstuff.nsmerge(allargs, args)

            # create the box!
            # note that name may be in recipe.toml (root only)
            box = Box(args.name, path=path, recipe=recipe, **{
                k: v for k, v in args.__dict__.items()
                if k not in {
                    'name', 'path', 'recipe', 'boxes', 'super', 'all'}})

            # scan parent/children
            if parent:
                assert not args.super, (
                    "Child box can't have parent (%s)" % box.name)
                box.parent = parent
            elif args.super:
                assert len(args.super) == 1, (
                    "A box can only have one superbox (%s)" %
                        ', '.join(args.super.keys()))
                name, boxargs = next(iter(args.super.items()))
                # create parent
                box.parent = recurse(**{**boxargs.__dict__, **dict(
                    name=name,
                    prefix=(prefix or '--') + 'super.%s.' % name,
                    path=os.path.join(path,
                        getattr(boxargs, 'path', None) or name),
                    child=box,
                    parentallargs=allargs)})

            boxes = []
            if child:
                assert all(name != child.name for name in args.box.items()), (
                    "Parent box can't have child with same name (%s)" % name)
                boxes.append(child)
            for name, boxargs in sorted(args.box.items()):
                # create child
                boxes.append(recurse(**{**boxargs.__dict__, **dict(
                    name=name,
                    prefix=(prefix or '--') + 'boxes.%s.' % name,
                    path=os.path.join(path,
                        getattr(boxargs, 'path', None) or name),
                    parent=box,
                    parentallargs=allargs)}))
            box.boxes = sorted(boxes)

            if box.parent:
                assert all(child.name != box.parent.name
                    for child in box.boxes), (
                    "Conflicting names between box's parent/child %r" %
                    box.parent.name)

            return box

        box = recurse(name, **args)

        # unzip the box
        while box.parent:
            box = box.parent

        return box

    scan.__func__.__argparse__ = _scan_argparse.__func__

    def box(self, stage=None):
        """
        Apply any post-init configuration that needs to know the full
        tree of boxes.

        Needs to:
        - Allocate box memory
        - Allocate sections
        - Create implicit exports?
        """
        if not stage or stage == 'boxes':
            # create memory slices for children
            for child in self.boxes:
                for memory in child.memories:
                    if memory.addr is None:
                        slice = self.consume(
                            mode=memory.mode,
                            size=memory.size,
                            align=memory.align,
                            reverse=True)
                        memory.addr = slice.addr

                    self.memoryslices = list(it.chain.from_iterable(
                        slice - memory for slice in self.memoryslices))

                # sort again in case new addresses changed order
                child.memories = sorted(child.memories)

            # make slice names unique
            namecount = {}
            for slice in self.memoryslices:
                namecount[slice.name] = namecount.get(slice.name, 0) + 1
            for name, count in namecount.items():
                if count > 1:
                    for i, slice in enumerate(
                            slice for slice in self.memoryslices
                            if slice.name == name):
                        slice.name = '%s%d' % (name, i+1)

            for child in self.boxes:
                child.box(stage='boxes')

        if not stage or stage == 'runtimes':
            self.runtime.box(self)
            for child in self.boxes:
                child.box(stage='runtimes')

        if not stage or stage == 'outputs':
            for output in self.outputs:
                output.box(self)
            for child in self.boxes:
                child.box(stage='outputs')

        if not stage or stage == 'epilogues':
            for child in self.boxes:
                child.box(stage='epilogues')
            for epilogue in self.box_epilogues.values():
                epilogue()

    def link(self):
        """
        Link together import/exports from boxes.
        """
        # first deduplicate exports and imports (may have been created
        # during boxing)
        importset = {}
        for import_ in self.imports:
            if import_.name in importset:
                assert importset[import_.name].iscompatible(import_), (
                    "Incompatible imports for %r:\n"
                    "import.%s = %s in %s\n"
                    "import.%s = %s in %s" % (
                    import_.name,
                    importset[import_.name].name,
                    importset[import_.name],
                    importset[import_.name].source.name,
                    import_.name,
                    import_,
                    import_.source.name))
            # prioritize our own imports
            if import_ not in importset or import_.source == self:
                importset[import_.name] = import_
        # actually don't make this unique, 
        self.imports = sorted(self.imports)

        exportset = {}
        for export in self.exports:
            if export.name in exportset:
                assert export.weak or exportset[export.name].weak, (
                    "Conflicting exports for %r:\n"
                    "export.%s = %s in %s\n"
                    "export.%s = %s in %s" % (
                    export.name,
                    exportset[export.name].name,
                    exportset[export.name],
                    exportset[export.name].source.name,
                    export.name,
                    export,
                    export.source.name))
                assert exportset[export.name].iscompatible(export), (
                    "Incompatible exports for %r:\n"
                    "export.%s = %s in %s\n"
                    "export.%s = %s in %s" % (
                    export.name,
                    exportset[export.name].name,
                    exportset[export.name],
                    exportset[export.name].source.name,
                    export.name,
                    export,
                    export.source.name))
            if export not in exportset or exportset[export.name].weak:
                exportset[export.name] = export
        self.exports = sorted(exportset.values())

        # now create linkages
        for export in self.exports:
            assert (export.targetname is None or any(
                target.name == export.targetname
                for target in it.chain(
                    [self],
                    [self.parent] if self.parent else [],
                    self.boxes))), (
                "No target %r found for export %r:\n"
                "export.%s = %s in %s" % (
                export.targetname, export.name,
                export.name, export, export.source.name))

        for import_ in self.imports:
            targets = []
            for target in it.chain(
                    [self],
                    [self.parent] if self.parent else [],
                    self.boxes):
                for export in target.exports:
                    if import_.islinkable(export, target, self):
                        targets.append(export)

            assert targets or import_.weak, (
                "No export found for %r:\n"
                "import.%s = %s in %s" % (
                import_.name,
                import_.name, import_, import_.source.name))
            assert len(targets) <= 1, (
                "Ambiguous import/export for %r:\n"
                "import.%s = %s in %s\n%s" % (
                import_.name,
                import_.name, import_, import_.source.name,
                '\n'.join("export.%s = %s in %s" % (
                    export.name, export, export.source.name)
                    for export in targets)))

            if targets:
                export = targets[0]
                assert export.iscompatible(import_), (
                    "Incompatible import/export for %r:\n"
                    "export.%s = %s in %s\n"
                    "import.%s = %s in %s" % (
                    export.name,
                    export.name,
                    export,
                    export.source.name,
                    import_.name,
                    import_,
                    import_.source.name))

                link = Link(export, import_)
                import_.link = link
                export.links.append(link)

        # link children
        for child in self.boxes:
            child.link()

        # create "n" functions. these generate compact unique numbers
        # for boxes/links/whatever
        for export in self.exports:
            def n(self):
                def n(box=None):
                    for n, export in enumerate(
                            export for export in self.box.exports
                            if not box or any(
                                link.import_.box == box
                                for link in export.links)):
                        if export is self:
                            return n
                return n
            export.box = self
            export.n = n(export)

        for import_ in self.imports:
            def n(self):
                def n(box=None):
                    for n, import_ in enumerate(
                            import_ for import_ in self.box.imports
                            if not box or import_.link.export.box == box):
                        if import_ is self:
                            return n
                return n
            import_.box = self
            import_.n = n(import_)

        for child in self.boxes:
            def n(self):
                def n(runtime=None):
                    if not self.parent:
                        return 0
                    for n, box in enumerate(
                            box for box in self.parent.boxes
                            if not runtime or box.runtime == runtime):
                        if box is self:
                            return n
                return n
            child.n = n(child)

    def build(self, stage=None):
        """
        Generate output data based on configured runtimes and outputs.
        """
        if not stage or stage == 'runtimes':
            self.runtime.build(self)
            for child in self.boxes:
                child.build(stage='runtimes')

        if not stage or stage == 'outputs':
            for child in self.boxes:
                child.build(stage='outputs')
            for output in self.outputs:
                output.build(self)

        if not stage or stage == 'epilogues':
            for child in self.boxes:
                child.build(stage='epilogues')
            for epilogue in self.build_epilogues.values():
                epilogue()
