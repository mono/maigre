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
#include <gtk/gtk.h>

#include "maigre-mono-bridge.h"

MaigreMonoBridge *
maigre_mono_bridge ()
{
    static MaigreMonoBridge *bridge;

    MonoMethod *theme_create_method;

    if (bridge != NULL) {
        return bridge;
    }

    bridge = g_new0 (MaigreMonoBridge, 1);

    bridge->domain = mono_domain_get ();

    if (bridge->domain == NULL) {
        mono_config_parse (NULL);
        bridge->domain = mono_jit_init (".run/engines/Maigre.dll");
    }

    if ((bridge->assembly = mono_domain_assembly_open (
            bridge->domain, ".run/engines/Maigre.dll")) == NULL ||
        (bridge->image = mono_assembly_get_image (bridge->assembly)) == NULL) {
        g_warning ("Could not load Maigre.dll assembly");
        return bridge;
    }

    if ((bridge->theme_class = mono_class_from_name (
            bridge->image, "Maigre", "Theme")) == NULL) {
        g_warning ("Maigre.dll assembly does not contain Maigre.Theme");
        return bridge;
    }

    if ((theme_create_method = mono_class_get_method_from_name (
            bridge->theme_class, "Create", 0)) == NULL) {
        g_warning ("Maigre.Theme does not contain a Create method");
        return bridge;
    }

    if ((bridge->theme_object = mono_runtime_invoke (theme_create_method,
            NULL, NULL, NULL)) == NULL) {
        g_warning ("Could not create an instance of Maigre.Theme");
        return bridge;
    }

    bridge->init_success = TRUE;

    return bridge;
}
