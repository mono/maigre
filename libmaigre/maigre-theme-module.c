//
// maigre-theme-module.c
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

#include <gmodule.h>
#include <gtk/gtk.h>

#include "maigre-rc-style.h"
#include "maigre-style.h"
#include "maigre-mono-bridge.h"

G_MODULE_EXPORT void
theme_init (GTypeModule *module)
{
    MaigreMonoBridge *bridge = maigre_mono_bridge ();
    MonoClass *klass;
    MonoMethod *method;

    if (bridge == NULL) {
        g_warning ("Maigre failed to initialize. Default internal "
            "GtkRcStyle/GtkStyle will be used.");
        return;
    }

    maigre_rc_style_register_types (module);

    if (bridge != NULL &&
        (klass = mono_class_from_name (bridge->image, "Maigre", "ThemeModule")) != NULL &&
        (method = mono_class_get_method_from_name (klass, "Init", 0)) != NULL) {
        mono_runtime_invoke (method, NULL, NULL, NULL);
   }
}

G_MODULE_EXPORT void
theme_exit ()
{
    MaigreMonoBridge *bridge = maigre_mono_bridge ();
    MonoClass *klass;
    MonoMethod *method;

    if (bridge != NULL &&
        (klass = mono_class_from_name (bridge->image, "Maigre", "ThemeModule")) != NULL &&
        (method = mono_class_get_method_from_name (klass, "Exit", 0)) != NULL) {
        mono_runtime_invoke (method, NULL, NULL, NULL);
   }
}

G_MODULE_EXPORT GtkRcStyle *
theme_create_rc_style ()
{
    return maigre_mono_bridge () != NULL
        ? GTK_RC_STYLE (g_object_new (MAIGRE_TYPE_RC_STYLE, NULL))
        : GTK_RC_STYLE (g_object_new (GTK_TYPE_RC_STYLE, NULL));
}

G_MODULE_EXPORT const gchar *
g_module_check_init (GModule *module)
{
    return gtk_check_version (
        GTK_MAJOR_VERSION,
        GTK_MINOR_VERSION,
        GTK_MICRO_VERSION - GTK_INTERFACE_AGE);
}
