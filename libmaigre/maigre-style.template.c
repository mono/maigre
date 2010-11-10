//
// maigre-style.c
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

#include <string.h>

#include "maigre-style.h"
#include "maigre-mono-bridge.h"

struct MaigreStyle {
    GtkStyle parent_instance;
    MonoObject *mono_object;
};

struct MaigreStyleClass {
    GtkStyleClass parent_class;
    MonoClass *mono_class;
};

G_DEFINE_TYPE (MaigreStyle, maigre_style, GTK_TYPE_STYLE);

typedef struct MaigreDrawContext {
% for member in parser.context_struct:
    ${member.to_native_string ()};
% endfor
} MaigreDrawContext;

typedef struct MaigreParentVTable {
% for method in parser.methods:
    gpointer ${method.native_name};
% endfor
} MaigreParentVTable;

% for method in parser.methods:
static void
${generator.wrap ('maigre_style_%s (%s)' % \
    (method.native_name, ', '.join (method.to_native_signature ())), 0, 1)}
{
    static MonoMethod *managed_method = NULL;

    MaigreMonoBridge *bridge;
    MaigreDrawContext draw_context, *draw_context_ptr;
    gpointer args[1];
    
    bridge = maigre_mono_bridge ();
    if (bridge == NULL) {
        return;
    } else if (managed_method == NULL) {
        managed_method = mono_class_get_method_from_name (
            MAIGRE_STYLE_GET_CLASS (style)->mono_class,
            "${method.managed_name}", 1);
        if (managed_method == NULL) {
            g_warning ("Maigre.Style does not contain a ${method.managed_name} method");
            return;
        }
    }

    memset (&draw_context, 0, sizeof (MaigreDrawContext));

    draw_context.method = "${method.managed_name}";
    % for arg in method.arguments:
    draw_context.${arg.native_name} = ${arg.native_name};
    % endfor

    draw_context_ptr = &draw_context;
    args[0] = &draw_context_ptr;
    mono_runtime_invoke (managed_method, MAIGRE_STYLE (style)->mono_object, args, NULL);
}

% endfor
static void
maigre_style_init (MaigreStyle *maigre)
{
    static MonoMethod *managed_method = NULL;

    gpointer args[1];

    if (managed_method == NULL) {
        managed_method = mono_class_get_method_from_name (
            MAIGRE_STYLE_GET_CLASS (maigre)->mono_class,
            "New", 1);
        if (managed_method == NULL) {
            g_warning ("Maigre.Style does not contain a Style:New(IntPtr) method");
            return;
        }
    }

    args[0] = &maigre;
    maigre->mono_object = mono_runtime_invoke (managed_method, NULL, args, NULL);
    g_object_ref (maigre);
}

static void
maigre_style_class_init (MaigreStyleClass *klass)
{
    MaigreMonoBridge *bridge;
    MonoMethod *configure_class_method;
    MaigreParentVTable parent_vtable, *parent_vtable_ptr;
    gpointer args[2];
    GType type = maigre_style_get_type ();

    bridge = maigre_mono_bridge ();
    if (bridge == NULL) {
        return;
    } else if ((klass->mono_class = mono_class_from_name (bridge->image,
        "Maigre", "Style")) == NULL) {
        g_warning ("Maigre.dll assembly does not contain Maigre.Style");
        return;
    } else if ((configure_class_method = mono_class_get_method_from_name (
        klass->mono_class, "ConfigureClass", 2)) == NULL) {
        g_warning ("Maigre.Style does not contain a ConfigureClass method.");
        return;
    }

    % for method in parser.methods:
    parent_vtable.${method.native_name} = GTK_STYLE_CLASS (maigre_style_parent_class)->${method.native_name};
    % endfor

    parent_vtable_ptr = &parent_vtable;
    args[0] = &parent_vtable_ptr;
    args[1] = &type;
    mono_runtime_invoke (configure_class_method, NULL, args, NULL);

    % for method in parser.methods:
    GTK_STYLE_CLASS (klass)->${method.native_name} = maigre_style_${method.native_name};
    % endfor
}
