// 
// maigre-rc-style.c
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

#include "maigre-rc-style.h"
#include "maigre-style.h"
#include "maigre-mono-bridge.h"

struct MaigreRcStyle {
    GtkRcStyle parent_instance;
    MonoObject *mono_object;
};

struct MaigreRcStyleClass {
    GtkRcStyleClass parent_class;
    MonoClass *mono_class;
};

G_DEFINE_DYNAMIC_TYPE (MaigreRcStyle, maigre_rc_style, GTK_TYPE_RC_STYLE);

void
maigre_rc_style_register_types (GTypeModule *module)
{
    maigre_rc_style_register_type (module);
}

static GtkStyle *
maigre_rc_style_create_style (GtkRcStyle *rc_style)
{
    return GTK_STYLE (g_object_new (MAIGRE_TYPE_STYLE, NULL));
}

static void
maigre_rc_style_init (MaigreRcStyle *maigre_rc)
{
    static MonoMethod *managed_method = NULL;

    gpointer args[1];

    if (managed_method == NULL) {
        managed_method = mono_class_get_method_from_name (
            MAIGRE_RC_STYLE_GET_CLASS (maigre_rc)->mono_class,
            "New", 1);
        if (managed_method == NULL) {
            g_warning ("Maigre.RcStyle does not contain a Style:New(IntPtr) method");
            return;
        }
    }

    args[0] = &maigre_rc;
    maigre_rc->mono_object = mono_runtime_invoke (managed_method, NULL, args, NULL);
    g_object_ref (maigre_rc);
}

static void
maigre_rc_style_class_init (MaigreRcStyleClass *klass)
{
    MaigreMonoBridge *bridge;
    MonoMethod *configure_class_method;
    gpointer args[1];
    GType type = maigre_rc_style_get_type ();

    bridge = maigre_mono_bridge ();
    if (bridge == NULL) {
        return;
    } else if ((klass->mono_class = mono_class_from_name (bridge->image,
        "Maigre", "RcStyle")) == NULL) {
        g_warning ("Maigre.dll assembly does not contain Maigre.RcStyle");
        return;
    } else if ((configure_class_method = mono_class_get_method_from_name (
        klass->mono_class, "ConfigureClass", 1)) == NULL) {
        g_warning ("Maigre.RcStyle does not contain a ConfigureClass method.");
        return;
    }

    args[0] = &type;
    mono_runtime_invoke (configure_class_method, NULL, args, NULL);

    GTK_RC_STYLE_CLASS (klass)->create_style = maigre_rc_style_create_style;
}

static void
maigre_rc_style_class_finalize (MaigreRcStyleClass *klass)
{
}
