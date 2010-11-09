#!/usr/bin/env python
#
# maigre-codegen.py
#
# Author:
#   Aaron Bockover <abockover@novell.com>
#
# Copyright 2010 Novell, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

def get_managed_name (native_name):
    managed_name = ''
    for part in native_name.split ('_'):
        managed_name += part.capitalize ()
    return managed_name

def get_managed_type (native_type, is_pointer):
    map = {
        'guint': 'uint',
        'gint': 'int',
        'gboolean': 'bool'
    }

    if native_type in map:
        return map[native_type]
    elif is_pointer and native_type == 'const gchar':
        return 'string'

    i = 0
    for c in native_type:
        if i > 1 and c.isupper ():
            break
        i += 1

    if i == len (native_type):
        return native_type

    return native_type[0:i] + '.' + native_type[i:]

class Argument:
    def __init__ (self, type, name, is_pointer):
        self.native_type = type
        self.native_name = name
        self.managed_type = get_managed_type (type, is_pointer)
        self.managed_name = get_managed_name (name)
        self.is_pointer = is_pointer
        self.is_custom_marshal = is_pointer and not self.managed_type == 'string'

    def to_native_string (self):
        string = self.native_type + ' '
        if self.is_pointer:
            string += '*'
        return string + self.native_name

    def to_managed_string (self, use_marshal_type = False):
        if self.is_custom_marshal and not use_marshal_type:
            return 'IntPtr ' + self.managed_name
        else:
            return self.managed_type + ' ' + self.managed_name

class Method:
    def __init__ (self, name):
        self.native_name = name
        self.managed_name = get_managed_name (name)
        self.arguments = []

    def to_native_signature (self):
        signature = []
        for arg in self.arguments:
            signature.append (arg.to_native_string ())
        return signature

    def to_managed_signature (self):
        signature = []
        for arg in self.arguments:
            signature.append (arg.to_managed_string ())
        return signature

class GtkStyleHeaderParser:
    def __init__ (self):
        self.methods = []
        self.blacklist = []
        self.context_struct = []

    def parse (self):
        last_word = None
        vmethod = None
        arg_type = None

        for word in open ('/usr/include/gtk-2.0/gtk/gtkstyle.h').read ().split ():
            if not last_word:
                last_word = word
                continue

            if last_word == 'void' and word.startswith ('(*draw_'):
                vmethod = Method (word.strip ('(*)'))
            elif vmethod:
                is_end = word.endswith (');')
                word = word.strip ('();,').strip ()
                if word == 'const':
                    arg_type = word
                    continue
                if not arg_type:
                    arg_type = word
                elif arg_type == 'const':
                    arg_type = 'const %s' % word
                else:
                    is_pointer = False
                    if word[0] == '*':
                        word = word[1:].strip ()
                        is_pointer = True
                    if word[0] == '_':
                        word = word[1:]
                    if word[-1] == '_':
                        word = word[0:-1]
                    vmethod.arguments.append (Argument (arg_type, word, is_pointer))
                    arg_type = None
                if is_end:
                    if vmethod.native_name not in self.blacklist:
                        self.methods.append (vmethod)
                    vmethod = None

            last_word = word

        self.build_context_struct ()

    def build_context_struct (self):
        self.context_struct = []
        self.context_struct.append (Argument ('const gchar', 'method', True))

        for method in self.methods:
            for arg in method.arguments:
                found = False
                for member in self.context_struct:
                    if arg.native_name == member.native_name and \
                        arg.native_type == member.native_type and \
                        arg.is_pointer == member.is_pointer:
                        found = True
                        break
                if not found:
                    self.context_struct.append (arg)

    def get_context_struct_marshallers (self):
        return filter (lambda member: member.is_custom_marshal, self.context_struct)

class CodeGenerator:
    def __init__ (self, stream, parser):
        self.stream = stream
        self.indent_size = 4
        self.parser = parser

    def wl (self, line = None, indent = 0):
        if line:
            self.w (line, indent)
        self.stream.write ('\n')

    def w (self, string, indent = 0):
        self.stream.write (self.indent (indent))
        self.stream.write (string)

    def ws (self, string, indent = 0):
        import textwrap
        i = 0
        lines = textwrap.dedent (string).split ('\n')
        for line in lines:
            if (i == 0 or i >= len (lines) - 2) and line.strip () == '':
                continue
            self.wl (line, indent)
            i += 1

    def indent (self, level = 0):
        s = ''
        for i in range (0, self.indent_size * level):
            s += ' '
        return s

    def wrap (self, string, indent = 0, subsequent_indent = 1, max_width = 70):
        from textwrap import TextWrapper
        import re
        TextWrapper.wordsep_re = re.compile (r'\s*(\,)')
        wrapper = TextWrapper (width = max_width)
        wrapped = ''
        i = 0
        for line in wrapper.wrap (string):
            line = line.strip ()
            if i > 0:
                if line[0] == ',':
                    line = line[1:].strip ()
                    wrapped += ',\n'
                else:
                    wrapped += '\n'
                wrapped += self.indent (subsequent_indent)
            wrapped += line
            i += 1
        return self.indent (indent) + wrapped.strip ()

class NativeCodeGenerator (CodeGenerator):
    def generate (self):
        self.ws ('''
            // This file was generated by the Maigre code generator.
            // Any changes made will be lost if regenerated.

            typedef struct MaigreDrawContext {
        ''')

        for member in self.parser.context_struct:
            self.wl ('%s;' % member.to_native_string (), 1)
        self.wl ('} MaigreDrawContext;')
        self.wl ()

        self.wl ('typedef struct MaigreParentVTable {')
        for method in self.parser.methods:
            self.wl ('gpointer %s;' % method.native_name, 1)
        self.wl ('} MaigreParentVTable;')
        self.wl ()

        for method in self.parser.methods:
            self.wl ('static void')
            self.wl (self.wrap ('maigre_style_%s (%s)' % \
                (method.native_name, ', '.join (method.to_native_signature ())),
                0, 1))
            self.ws ('''
            {
                static MonoMethod *managed_method = NULL;

                MaigreMonoBridge *bridge;
                MaigreDrawContext draw_context, *draw_context_ptr;
                gpointer args[1];

                bridge = maigre_mono_bridge ();
                if (bridge == NULL || !bridge->init_success) {
                    return;
                } else if (managed_method == NULL) {
                    managed_method = mono_class_get_method_from_name (
                        bridge->theme_class, "Proxy%s", 1);
                    if (managed_method == NULL) {
                        g_warning ("Maigre.Theme does not contain a Proxy%s method");
                        return;
                    }
                }

                memset (&draw_context, 0, sizeof (MaigreDrawContext));

                draw_context.method = "%s";
            ''' % (method.managed_name, method.managed_name, method.managed_name))

            for arg in method.arguments:
                self.wl ('draw_context.%s = %s;' % (arg.native_name, arg.native_name), 1)

            self.wl ()
            self.ws ('''
                draw_context_ptr = &draw_context;
                args[0] = &draw_context_ptr;
                mono_runtime_invoke (managed_method, bridge->theme_object, args, NULL);
            }
            ''')
            self.wl ()

        self.ws ('''
            static void
            maigre_style_override_methods (GtkStyleClass *klass)
            {
                MaigreMonoBridge *bridge;
                MonoMethod *load_parent_vtable_method;
                MaigreParentVTable parent_vtable, *parent_vtable_ptr;
                gpointer args[1];

                bridge = maigre_mono_bridge ();
                if (bridge == NULL || !bridge->init_success) {
                    return;
                } else if ((load_parent_vtable_method = mono_class_get_method_from_name (
                        bridge->theme_class, "LoadParentVTable", 1)) == NULL) {
                    g_warning ("Maigre.Theme does not contain a LoadParentVTable method.");
                    return;
                }

        ''')

        for method in self.parser.methods:
            self.wl ('parent_vtable.%s = GTK_STYLE_CLASS (maigre_style_parent_class)->%s;' \
                % (method.native_name, method.native_name), 1)

        self.wl ()
        self.ws ('''
            parent_vtable_ptr = &parent_vtable;
            args[0] = &parent_vtable_ptr;
            mono_runtime_invoke (load_parent_vtable_method, bridge->theme_object, args, NULL);
        ''', 1)
        self.wl ()

        for method in self.parser.methods:
            self.wl ('klass->%s = maigre_style_%s;' % (method.native_name,
                method.native_name), 1)

        self.wl ('}')

class ManagedCodeGenerator (CodeGenerator):
    def generate (self):
        self.ws ('''
            // This file was generated by the Maigre code generator.
            // Any changes made will be lost if regenerated.

            using System;
            using System.Runtime.InteropServices;

            namespace Maigre
            {
                public abstract partial class Theme
                {
                    [StructLayout (LayoutKind.Sequential)]
                    private struct DrawContext
                    {
        ''')

        for member in self.parser.context_struct:
            self.wl ('public %s;' % member.to_managed_string (), 3)

        self.wl ('}', 2)
        self.wl ()

        self.ws ('''

            [StructLayout (LayoutKind.Sequential)]
            private struct ParentVTable
            {''', 2)

        for method in self.parser.methods:
            self.wl ('public %sHandler %s;' % \
                (method.managed_name, method.managed_name), 3)

        self.wl ('}', 2)
        self.wl ()

        for method in self.parser.methods:
            self.wl (self.wrap ('private delegate void %sHandler (%s);' % \
                (method.managed_name, ', '.join (method.to_managed_signature ())), 2, 3))
            self.wl ()

        self.wl ()
        self.ws ('''
            private DrawContext context;
            private ParentVTable parent_vtable;

            private Cairo.Context cr;
            protected Cairo.Context Cr {
                get { return cr; }
            }

            protected Cairo.Rectangle Shape { get; private set; }

        ''', 2)

        first_non_marshalled = False
        for member in sorted (self.parser.context_struct,
            key = lambda m: not m.is_custom_marshal):
            if not member.is_custom_marshal and not first_non_marshalled:
                self.wl ()
                first_non_marshalled = True
            self.w ('public %s {' % member.to_managed_string (True), 2)
            if member.is_custom_marshal:
                self.wl (' get; private set; }')
            else:
                self.wl ()
                self.wl ('get { return context.%s; }' % member.managed_name, 3)
                self.wl ('}', 2)
                self.wl ()

        self.ws ('''
            private void LoadParentVTable (IntPtr vtablePtr)
            {
                parent_vtable = (ParentVTable)Marshal.PtrToStructure (vtablePtr, typeof (ParentVTable));
            }

            private void LoadContext (IntPtr ctxPtr)
            {
                context = (DrawContext)Marshal.PtrToStructure (ctxPtr, typeof (DrawContext));

                Shape = new Cairo.Rectangle (0, 0, Width, Height);

        ''', 2)

        i = 0
        members = self.parser.get_context_struct_marshallers ()
        for member in members:
            if member.managed_type == 'Gdk.Rectangle':
                self.ws ('''
                    %s = context.%s != IntPtr.Zero
                        ? (Gdk.Rectangle)Marshal.PtrToStructure (context.%s, typeof (Gdk.Rectangle))
                        : Gdk.Rectangle.Zero;
                ''' % (member.managed_name, member.managed_name, member.managed_name), 3)
            else:
                self.ws ('''
                    %s = context.%s != IntPtr.Zero
                        ? (%s)GLib.Object.GetObject (context.%s)
                        : null;
                ''' % (member.managed_name, member.managed_name,
                        member.managed_type, member.managed_name), 3)
            if i < len (members) - 1:
                self.wl ()
                i += 1

        self.wl ('}', 2)
        self.wl ()

        i = 0
        for method in self.parser.methods:
            self.ws ('''
                protected virtual void %s ()
                {
                    if (parent_vtable.%s != null) {
                        parent_vtable.%s (
            ''' % (method.managed_name, method.managed_name, method.managed_name), 2)
            i = 0
            for arg in method.arguments:
                self.w ('context.%s' % arg.managed_name, 5)
                if i < len (method.arguments) - 1:
                    self.w (',')
                i += 1
                self.wl ()
            self.ws ('''
                        );
                    }
                }

                private void Proxy%s (IntPtr ctxPtr)
                {
                    LoadContext (ctxPtr);

                    using (cr = Gdk.CairoHelper.Create (Window)) {
                        cr.Translate (X, Y);
                        %s ();
                    }

                    cr = null;
                }
            ''' % (method.managed_name, method.managed_name), 2)
            if i < len (self.parser.methods) - 1:
                self.wl ()
                i += 1

        self.wl ('}', 1)
        self.wl ('}')

parser = GtkStyleHeaderParser ()
parser.blacklist = ['draw_polygon']
parser.parse ()

NativeCodeGenerator (
    open ('../libmaigre/maigre-style-overrides.c', 'w'),
    parser).generate ()

ManagedCodeGenerator (
    open ('../Maigre/Maigre/Theme_Generated.cs', 'w'),
    parser).generate ()
