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

last_word = None
vmethod = None
arg_type = None
methods = []
draw_context = []

blacklist = ['draw_polygon']

def get_managed_name (native_name):
    managed_name = ''
    for part in native_name.split ('_'):
        managed_name += part.capitalize ()
    return managed_name

out = open ('../libmaigre/maigre-style-overrides.c', 'w')

for word in open ('/usr/include/gtk-2.0/gtk/gtkstyle.h').read ().split ():
    if not last_word:
        last_word = word
        continue

    if last_word == 'void' and word.startswith ('(*draw_'):
        vmethod = word.strip ('(*)')
        methods.append ([vmethod])
    elif vmethod:
        end = word.endswith (');')
        word = word.strip ('();,')
        if word == 'const':
            arg_type = word
            continue
        if not arg_type:
            arg_type = word
        elif arg_type == 'const':
            arg_type = 'const %s' % word
        else:
            word = word.strip ()
            if word[0] == '*':
                word = word[1:]
                arg_type += ' *'
            if word[0] == '_':
                word = word[1:]
            methods[-1].append ((arg_type, word))
            arg_type = None
        if end:
            vmethod = None

    last_word = word

filtered_methods = []
for method in methods:
    if method[0] not in blacklist:
        filtered_methods.append (method)
methods = filtered_methods

draw_context.append (('const gchar *', 'method'))
for method in methods:
    for arg in method[1:]:
        found = False
        for item in draw_context:
            if item[0] == arg[0] and item[1] == arg[1]:
                found = True
                break
        if not found:
            draw_context.append ((arg[0], arg[1]))

out.write ('''// This file was generated by the Maigre code generator.
// Any changes made will be lost if regenerated.

typedef struct MaigreDrawContext {
''')

for item in draw_context:
    line = item[0]
    if line.endswith (' *'):
        line = item[0] + item[1]
    else:
        line = item[0] + ' ' + item[1]
    out.write ('    %s;\n' % line)

out.write ('} MaigreDrawContext;\n')

method_index = 0
for method in methods:
    line_count = 0
    arg_index = 0
    arg_count = len (method[1:])
    args = method[1:]
    arg_names = []
    managed_method = get_managed_name (method[0])
    for arg in args:
        arg_names.append (arg[1])

    out.write ('static void\n')
    line = 'maigre_style_%s (' % method[0]
    for arg in method[1:]:
        if arg[0][-1] == '*':
            line += '%s%s' % (arg[0], arg[1])
        else:
            line += '%s %s' % (arg[0], arg[1])
        if arg_index < arg_count - 1:
            line += ', '
        else:
            line += ')'
        arg_index += 1
        if len (line) > 55:
            line_count += 1
            out.write (line + '\n')
            line = '    '
    if len (line.strip ()) > 0:
        out.write (line + '\n')
    out.write ('''{
    static MonoMethod *managed_method = NULL;

    MaigreMonoBridge *bridge;
    MaigreDrawContext draw_context, *draw_context_ptr;
    gpointer args[1];
''')

    out.write ('''
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
''' % (managed_method, managed_method))

    out.write ('''
    memset (&draw_context, 0, sizeof (MaigreDrawContext));

    draw_context.method = "%s";
''' % managed_method)

    for arg in method[1:]:
        var = arg[1]
        if var[0] == '*':
            var = var[1:]
        out.write ('    draw_context.%s = %s;\n' % (var, var))

    out.write ('''
    draw_context_ptr = &draw_context;
    args[0] = &draw_context_ptr;

    if (*(guchar *)mono_object_unbox (mono_runtime_invoke (
        managed_method, bridge->theme_object, args, NULL)) == 0) {
        GTK_STYLE_CLASS (maigre_style_parent_class)->%s (
''' % method[0])

    arg_index = 0
    for arg in method[1:]:
        var = arg[1]
        if var[0] == '*':
            var = var[1:]
        out.write ('            %s' % var)
        if arg_index < arg_count - 1:
            out.write (',\n')
        arg_index += 1

    out.write ('''
        );
    }
}

''')

out.write ('''
static void
maigre_style_override_methods (GtkStyleClass *klass)
{
''')
for method in methods:
    out.write ('    klass->%s = maigre_style_%s;\n' \
        % (method[0], method[0]))
out.write ('}')


managed_draw_context = []
for item in draw_context:
    gobject_type = None
    type = item[0]
    if type == 'const gchar *':
        type = 'string'
    elif type.startswith (('Gtk', 'Gdk', 'Pango')):
        ofs = 3
        if type.startswith ('Pango'):
            ofs = 5
        is_ptr = False
        if type[-1] == '*':
            is_ptr = True
        type = type[0:ofs] + '.' + type[ofs:]
        if is_ptr:
            gobject_type = type[0:-1].strip ()
            type = 'IntPtr'
        if gobject_type == 'Gdk.Point':
            gobject_type += ' []'
    map = {'guint':'uint', 'gint':'int', 'gboolean':'bool'}
    if type in map:
        type = map[type]
    managed_draw_context.append ((type, get_managed_name (item[1]),
        gobject_type, item[1]))

out = open ('../Maigre/Maigre/Theme_Generated.cs', 'w')
out.write ('''// This file was generated by the Maigre code generator.
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

for type, name, gobject_type, native_name in managed_draw_context:
    out.write ('            public %s %s;\n' % (type, name))

out.write ('''        }

        private DrawContext context;
        private bool gtk_style_draw_call_parent;

        private Cairo.Context cr;
        public Cairo.Context Cr {
            get { return cr; }
        }
''')

for type, name, gobject_type, native_name in managed_draw_context:
    return_stmt = 'context.%s' % name
    if gobject_type:
        type = gobject_type
        return_stmt = native_name
        out.write ('        private %s %s;\n' % (type, native_name))

    out.write ('''        public %s %s {
            get { return %s; }
        }

''' % (type, name, return_stmt))

out.write ('''        private void LoadContext (IntPtr ctxPtr)
        {
            context = (DrawContext)Marshal.PtrToStructure (ctxPtr, typeof (DrawContext));
''')

for type, name, gobject_type, native_name in managed_draw_context:
    if gobject_type == 'Gdk.Rectangle':
        out.write ('''
            %s = context.%s != IntPtr.Zero
                ? (Gdk.Rectangle)Marshal.PtrToStructure (context.%s, typeof (Gdk.Rectangle))
                : Gdk.Rectangle.Zero;
''' % (native_name, name, name))
    elif gobject_type:
        out.write ('''
            %s = context.%s != IntPtr.Zero
                ? (%s)GLib.Object.GetObject (context.%s)
                : null;
''' % (native_name, name, gobject_type, name))

out.write ('''        }

''')

method_index = 0
for method in methods:
    method = get_managed_name (method[0])
    out.write ('''        protected virtual void %s ()
        {
            gtk_style_draw_call_parent = true;
        }

        private bool Proxy%s (IntPtr ctxPtr)
        {
            LoadContext (ctxPtr);

            gtk_style_draw_call_parent = false;

            using (cr = Gdk.CairoHelper.Create (Window)) {
                cr.Translate (X, Y);
                %s ();
            }

            cr = null;

            return !gtk_style_draw_call_parent;
        }
''' % (method, method, method))
    if method_index < len (methods) - 1:
        out.write ('\n')
    method_index += 1
out.write ('''    }
}
''')