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
    static gboolean load_attempted;

    MonoMethod *theme_create_method;

    if (load_attempted) {
        return bridge;
    }

    load_attempted = TRUE;

    bridge = g_new0 (MaigreMonoBridge, 1);

    bridge->domain = mono_domain_get ();

    if (bridge->domain == NULL) {
        mono_config_parse (NULL);
        bridge->domain = mono_jit_init ("maigre-domain");
    }

    if ((bridge->assembly = mono_domain_assembly_open (
            bridge->domain, ".run/engines/Maigre.dll")) == NULL ||
        (bridge->image = mono_assembly_get_image (bridge->assembly)) == NULL) {
        g_warning ("Could not load Maigre.dll assembly");
        g_free (bridge);
        bridge = NULL;
    }

    return bridge;
}
