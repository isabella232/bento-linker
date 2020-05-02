from .. import outputs
from ..box import Fn
import io
import textwrap

@outputs.output('sys')
@outputs.output('box')
class HeaderOutput(outputs.Output):
    """
    Name of header file to generate containing the imported functions.
    """
    __argname__ = "header_glue"
    __arghelp__ = __doc__

    def __init__(self, sys, box, path):
        self._includes = []
        self._decls = []
        super().__init__(sys, box, path)

        if box:
            self.format(gaurd='__BOX_%s_H' % box.name.upper())
        else:
            self.format(gaurd='__BOX_H')

    def append_include(self, fmt=None, **kwargs):
        include = fmt % self.mkformat(**kwargs)
        if not include.startswith('"') and not include.startswith('<'):
            include = '"%s"' % include
        if include not in self._includes:
            self._includes.append(include)

    def append_decl(self, fmt=None, **kwargs):
        outf = self.mkfield(**kwargs)
        self._decls.append(outf)
        if fmt is not None:
            outf.write(fmt)
        return outf

    def build(self, outf):
        outf.write('////// AUTOGENERATED //////\n')
        outf.write('#ifndef %(gaurd)s\n' % self.mkformat())
        outf.write('#define %(gaurd)s\n' % self.mkformat())
        for include in self._includes:
            outf.write('#include %s\n' % include)
        outf.write('\n')
        for decl in self._decls:
            outf.write(decl.getvalue())
            outf.write('\n')
        outf.write('#endif\n')

@outputs.output('sys')
@outputs.output('box')
class HeaderGlueOutput_(outputs.Output_):
    """
    Name of header file to generate containing the imported functions.
    """
    __argname__ = "header_glue_"
    __arghelp__ = __doc__

    def __init__(self, sys, box):
        self.includes = outputs.OutputField_(self)
        self.decls = outputs.OutputField_(self)

        if box:
            super().__init__(
                box=box.name, BOX=box.name.upper(),
                gaurd='__BOX_%s_H' % box.name.upper())
        else:
            super().__init__(
                gaurd='__BOX_H' % box.name.upper())

        # TODO differentiate between sys/box???
        if box:
            self.decls.append('void __box_%(box)s_init(void);',
                doc='%(box)s box initialization')

            if box.imports:
                self.decls.append('\n//// box %(box)s imports ////')
            for import_ in box.imports:
                self.decls.append('extern %(fn)s;',
                    fn=import_.repr_c(), doc=import_.doc)
            if box.exports:
                self.decls.append('\n//// box %(box)s exports ////')
            for export in box.exports:
                self.decls.append('%(fn)s;',
                    fn=export.repr_c(), doc=export.doc)

    def build(self, outf):
        outf.write('////// AUTOGENERATED //////\n')
        outf.write('#ifndef %(gaurd)s\n' % self)
        outf.write('#define %(gaurd)s\n' % self)
        includes = set()
        for include in self.includes:
            include = include.getvalue()
            if not (include.startswith('"') or include.startswith('<')):
                include = '"%s"' % include
            includes.add(include)
        for include in sorted(includes):
            outf.write('#include %s\n' % include)
        outf.write('\n')

        for i, decl in enumerate(self.decls):
            if decl.get('doc', None) is not None:
                if i != 0:
                    outf.write('\n')
                for line in textwrap.wrap(decl['doc'] % decl, width=77):
                    outf.write('// %s\n' % line)
            decl.build(outf)
            outf.write('\n')

        outf.write('\n')
        outf.write('#endif\n')
    
