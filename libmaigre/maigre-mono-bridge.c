// 
// maigre-mono-bridge.c
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
#include <glib.h>

#include "maigre-mono-bridge.h"

static MonoMethod *
maigre_mono_bridge_load_ctor (MonoImage *image,
    const gchar *namespace, const gchar *class_name, const gchar *signature)
{
    MonoClass *klass;
    MonoMethodDesc *ctor_desc;
    MonoMethod *ctor;

    if ((klass = mono_class_from_name (image, namespace, class_name)) == NULL ||
        (ctor_desc = mono_method_desc_new (signature, FALSE)) == NULL ||
        (ctor = mono_method_desc_search_in_class (ctor_desc, klass)) == NULL) {
        return NULL;
    }

    mono_method_desc_free (ctor_desc);

    return ctor;
}

static void
maigre_mono_bridge_mono_assembly_loaded (MonoAssembly *assembly, MaigreMonoBridge *bridge)
{
    MonoImage *image = mono_assembly_get_image (assembly);
    const gchar *assembly_name = mono_image_get_name (image);

    if (bridge->assemblies_loaded) {
        return;
    } else if (strcmp ("gdk-sharp", assembly_name) == 0) {
        bridge->gdk_rectangle_ctor = maigre_mono_bridge_load_ctor
            (image, "Gdk", "Rectangle", ":.ctor(int,int,int,int)");
    } else if (strcmp ("glib-sharp", assembly_name) == 0) {
        bridge->glib_getobject = mono_class_get_method_from_name (
            mono_class_from_name (image, "GLib", "Object"),
            "GetObject",
            1);
    }

    bridge->assemblies_loaded =
        bridge->gdk_rectangle_ctor != NULL &&
        bridge->glib_getobject != NULL;
}

MaigreMonoBridge *
maigre_mono_bridge ()
{
    static MaigreMonoBridge *bridge;

    if (bridge != NULL) {
        return bridge;
    }

    bridge = g_new0 (MaigreMonoBridge, 1);

    mono_install_assembly_load_hook (
        (MonoAssemblyLoadFunc)maigre_mono_bridge_mono_assembly_loaded,
        bridge);

    bridge->domain = mono_domain_get ();

    if (bridge->domain == NULL) {
        mono_config_parse (NULL);
        bridge->domain = mono_jit_init ("Maigre.dll");
    }

    if ((bridge->assembly = mono_domain_assembly_open (
            bridge->domain, "Maigre.dll")) == NULL ||
        (bridge->image = mono_assembly_get_image (bridge->assembly)) == NULL) {
        g_warning ("Could not load Maigre.dll assembly");
        return bridge;
    }

    if ((bridge->theme_class = mono_class_from_name (
            bridge->image, "Maigre", "Theme")) == NULL) {
        g_warning ("Maigre.dll assembly does not contain Maigre.Theme");
        return bridge;
    }

    bridge->init_success = TRUE;

    return bridge;
}

MonoObject *
maigre_gobject_new (MaigreMonoBridge *bridge, gpointer native)
{
    gpointer args[1] = { &native };
    return native == NULL
        ? NULL
        : mono_runtime_invoke (bridge->glib_getobject, NULL, args, NULL);
}

MonoObject *
maigre_gdk_rectangle_new (MaigreMonoBridge *bridge, GdkRectangle *area)
{
    MonoClass *klass = mono_method_get_class (bridge->gdk_rectangle_ctor);

    if (area == NULL) {
        return mono_field_get_value_object (
            bridge->domain,
            mono_class_get_field_from_name (klass, "Zero"),
            NULL);
    } else {
        MonoObject *object;
        gpointer args[4] = { &area->x, &area->y, &area->width, &area->height };
        object = mono_object_new (bridge->domain, klass);
        mono_runtime_invoke (bridge->gdk_rectangle_ctor, object, args, NULL);
        return object;
    }
}
