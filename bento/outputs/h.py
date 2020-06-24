from .. import outputs
from ..box import Fn
import io
import textwrap

@outputs.output
class HOutput(outputs.Output):
    """
    Name of header file to generate containing the imported functions.
    """
    __argname__ = "h"
    __arghelp__ = __doc__

    def __init__(self, path=None):
        super().__init__(path)

        def buildinclude(out, include):
            if not (include.startswith('"') or include.startswith('<')):
                include = '"%s"' % include
            out.writef(include)

        def buildfn(out, fn):
            if fn.doc:
                out.pushattrs(doc=fn.doc)
            if out.get('attrs', False):
                out.writef('%(attrs)s ')
            out.writef('%(fn)s;', fn=fn.repr_c())

        self.includes = outputs.OutputField(self, {str: buildinclude})
        self.decls = outputs.OutputField(self, {Fn: buildfn})

    def box(self, box):
        super().box(box)
        self.pushattrs(gaurd='__BOX_%(BOX)s_H')

    def build_prologue(self, box):
        # always need standard types
        self.includes.append("<stdint.h>")
        self.includes.append("<stdbool.h>")
        self.includes.append("<sys/types.h>")

        # TODO configurable?
        out = self.decls.append()
        out.printf('//// box error codes ////')
        out.printf('enum box_err {')
        with out.indent():
            for name, code, doc in [
                ('BOX_ERR_OK',       0,    'No error'),
                ('BOX_ERR_GENERAL',  -1,   'General error'),
                ('BOX_ERR_NOEXEC',   -8,   'Box format error'),
                ('BOX_ERR_AGAIN',    -11,  'Try again'),
                ('BOX_ERR_NOMEM',    -12,  'Cannot allocate memory'),
                ('BOX_ERR_FAULT',    -14,  'Bad address'),
                ('BOX_ERR_BUSY',     -16,  'Device or resource busy'),
                ('BOX_ERR_LOOP',     -20,  'Cyclic data structure detected'),
                ('BOX_ERR_INVAL',    -22,  'Invalid parameter'),
                ('BOX_ERR_TIMEDOUT', -110, 'Timed out')]:
                out.printf('%(name)-24s = %(code)-5s // %(doc)s',
                    name=name,
                    code='%s,' % code,
                    doc=doc)
        out.printf('};')

        for i, import_ in enumerate(
                import_ for import_ in box.imports
                if import_.source == box):
            if i == 0:
                self.decls.append('//// box imports ////')
            self.decls.append(import_)

        for i, export in enumerate(
                export for export in box.exports
                if export.source == box):
            if i == 0:
                self.decls.append('//// box exports ////')
            self.decls.append(export, attrs='extern')

        # functions we can expect from runtimes
        self.decls.append('//// box hooks ////')
        self.decls.append(
            '__attribute__((noreturn))\n'
            'void __box_abort(int err);',
            doc='Forcefully terminate the current box with the specified '
                'error. The box can not be called again after this without '
                'a new init. Does not return.')
        self.decls.append(
            'ssize_t __box_write(int32_t fd, void *buffer, size_t size);',
            doc='Write to stdout if provided by superbox. If not provided, '
                'this function is still available for linking, but does '
                'nothing. Returns 0 on success, negative error code on '
                'failure.')
        for subbox in box.boxes:
            with self.pushattrs(box=subbox.name):
                self.decls.append(
                    'int __box_%(box)s_init(void);',
                    doc='Initialize box %(box)s.')
                self.decls.append(
                    'int __box_%(box)s_clobber(void);',
                    doc='Mark the box %(box)s as needing to be reinitialized.')

    def getvalue(self):
        self.seek(0)
        self.printf('////// AUTOGENERATED //////')
        self.printf('#ifndef %(gaurd)s')
        self.printf('#define %(gaurd)s')

        includes = set()
        for include in self.includes:
            includes.add(include.getvalue())
        out = self.decls.insert(0)
        for include in sorted(includes):
            out.print('#include %s' % include)

        for decl in self.decls:
            if 'doc' in decl:
                for line in textwrap.wrap(decl['doc'], width=78-3):
                    self.print('// %s' % line)
            self.print(decl.getvalue().strip())
            self.print()

        self.printf('#endif')

        return super().getvalue()
