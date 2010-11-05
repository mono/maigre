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

#include "maigre-style.h"
#include "maigre-mono-bridge.h"

#include "maigre-style-overrides.c"

typedef void (* DrawFnptr) ();

struct MaigreStyle {
    GtkStyle parent_instance;
};

struct MaigreStyleClass {
    GtkStyleClass parent_class;
};

G_DEFINE_DYNAMIC_TYPE (MaigreStyle, maigre_style, GTK_TYPE_STYLE);

void
maigre_style_register_types (GTypeModule *module)
{
    maigre_style_register_type (module);
}

static void
maigre_style_init (MaigreStyle *maigre)
{
}

static void
maigre_style_class_init (MaigreStyleClass *klass)
{
    MaigreMonoBridge *bridge;
    gint i;

    bridge = maigre_mono_bridge ();
    if (!bridge->init_success) {
        return;
    }

    maigre_style_init_method_map ();

    for (i = 0; maigre_style_method_map[i].name != NULL; i++) {
        MonoMethod *method = maigre_style_method_map[i].managed_method =
            mono_class_get_method_from_name (
                bridge->theme_class,
                maigre_style_method_map[i].name,
                maigre_style_method_map[i].arg_count);
        if (method != NULL) {
            DrawFnptr *slot = G_STRUCT_MEMBER_P (
                GTK_STYLE_CLASS (klass),
                G_STRUCT_OFFSET (GtkStyleClass, draw_hline) + i * sizeof (gpointer)
            );
            *slot = (DrawFnptr)maigre_style_method_map[i].native_method;
        }
    }
}

static void
maigre_style_class_finalize (MaigreStyleClass *klass)
{
}