// 
// maigre-mono-bridge.h
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

#ifndef MAIGRE_MONO_BRIDGE_H
#define MAIGRE_MONO_BRIDGE_H

#include <gdk/gdk.h>

#include <mono/jit/jit.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/image.h>
#include <mono/metadata/class.h>
#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>

// NOTE: this structure must reflect the fields
// in the Maigre.DrawContext managed structure
typedef struct MaigreMonoDrawContext {
    MonoClassField *method;
    MonoClassField *style;
    MonoClassField *window;
    MonoClassField *state_type;
    MonoClassField *shadow_type;
    MonoClassField *area;
    MonoClassField *widget;
    MonoClassField *detail;
    MonoClassField *x;
    MonoClassField *y;
    MonoClassField *width;
    MonoClassField *height;
    MonoClassField *orientation;
    MonoClassField *expander_style;
    MonoClassField *layout;
    MonoClassField *use_text;
    MonoClassField *edge;
    MonoClassField *step;
    MonoClassField *x1;
    MonoClassField *x2;
    MonoClassField *y1;
    MonoClassField *y2;
    MonoClassField *fill;
    MonoClassField *arrow_type;
    MonoClass *klass;
} MaigreMonoDrawContext;

typedef struct MaigreMonoBridge {
    gboolean init_success;
    gboolean assemblies_loaded;

    MonoDomain *domain;
    MonoImage *image;
    MonoAssembly *assembly;
    MonoClass *theme_class;

    MonoMethod *gdk_rectangle_ctor;
    MonoMethod *glib_getobject;

    MaigreMonoDrawContext draw_context;
} MaigreMonoBridge;

MaigreMonoBridge *maigre_mono_bridge ();

MonoObject *maigre_gobject_new (MaigreMonoBridge *bridge, gpointer native);
MonoObject *maigre_gdk_rectangle_new (MaigreMonoBridge *bridge, GdkRectangle *area);

#endif /* MAIGRE_MONO_BRIDGE_H */
