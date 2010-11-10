// 
// Theme.cs
//  
// Author:
//   Aaron Bockover <abockover@novell.com>
// 
// Copyright 2010 Novell, Inc.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

using System;
using System.Runtime.InteropServices;

namespace Maigre
{
    public abstract class Theme
    {
        public Theme ()
        {
        }

        private void ProxyModuleInit ()
        {
            ModuleInit ();
        }

        protected virtual void ModuleInit ()
        {
        }

        private void ProxyModuleExit ()
        {
            ModuleExit ();
        }

        protected virtual void ModuleExit ()
        {
        }

        public static Theme Create ()
        {
            return new Maigre.Osx.OsxTheme ();
        }

#region Generated

        [StructLayout (LayoutKind.Sequential)]
        private struct DrawContext
        {
            % for member in parser.context_struct:
            public ${member.to_managed_string ()};
            % endfor
        }

        [StructLayout (LayoutKind.Sequential)]
        private struct ParentVTable
        {
            % for method in parser.methods:
            public ${method.managed_name}Handler ${method.managed_name};
            % endfor
        }

        % for method in parser.methods:
        ${generator.wrap ('private delegate void %sHandler (%s);' % \
            (method.managed_name, ', '.join (method.to_managed_signature ())), 0, 3)}

        % endfor
        private DrawContext context;
        private ParentVTable parent_vtable;

        protected Cairo.Context Cr { get; private set; }
        protected Cairo.Rectangle Shape { get; private set; }

        % for member in sorted (parser.context_struct, \
          key = lambda m: not m.is_custom_marshal):
        % if member.is_custom_marshal:
        protected ${member.to_managed_string (True)} { get; private set; }
        % else:

        protected ${member.to_managed_string (True)} {
            get { return context.${member.managed_name}; }
        }
        % endif
        % endfor

        private void LoadParentVTable (IntPtr vtablePtr)
        {
            parent_vtable = (ParentVTable)Marshal.PtrToStructure (vtablePtr, typeof (ParentVTable));
        }

        private void LoadContext (IntPtr ctxPtr)
        {
            context = (DrawContext)Marshal.PtrToStructure (ctxPtr, typeof (DrawContext));

            Shape = new Cairo.Rectangle (0, 0, Width, Height);

        % for member in parser.get_context_struct_marshallers ():
        % if member.managed_type == 'Gdk.Rectangle':
            ${member.managed_name} = context.${member.managed_name} != IntPtr.Zero
                ? (Gdk.Rectangle)Marshal.PtrToStructure (context.${member.managed_name}, typeof (Gdk.Rectangle))
                : Gdk.Rectangle.Zero;
        % else:
            ${member.managed_name} = context.${member.managed_name} != IntPtr.Zero
                ? (${member.managed_type})GLib.Object.GetObject (context.${member.managed_name})
                : null;
        % endif

        % endfor
        }
        % for method in parser.methods:
        <% i = 0 %>
        protected virtual void ${method.managed_name} ()
        {
            if (parent_vtable.${method.managed_name} != null) {
                parent_vtable.${method.managed_name} (
                % for arg in method.arguments:
                <%
                    c = ''
                    if i < len (method.arguments) - 1:
                        c = ','
                        i += 1
                %>    context.${arg.managed_name}${c}
                % endfor
                );
            }
        }

        private void Proxy${method.managed_name} (IntPtr ctxPtr)
        {
            LoadContext (ctxPtr);

            using (Cr = Gdk.CairoHelper.Create (Window)) {
                Cr.Translate (X, Y);
                ${method.managed_name} ();
            }

            Cr = null;
        }
        % endfor

#endregion

    }
}
