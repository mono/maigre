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

struct MaigreRcStyle {
    GtkRcStyle parent_instance;
};

struct MaigreRcStyleClass {
    GtkRcStyleClass parent_class;
};

G_DEFINE_DYNAMIC_TYPE (MaigreRcStyle, maigre_rc_style, GTK_TYPE_RC_STYLE);

void
maigre_rc_style_register_types (GTypeModule *module)
{
    maigre_rc_style_register_type (module);
}

static void
maigre_rc_style_init (MaigreRcStyle *maigre_rc)
{
}

static GtkStyle *
maigre_rc_style_create_style (GtkRcStyle *rc_style)
{
    return GTK_STYLE (g_object_new (MAIGRE_TYPE_STYLE, NULL));
}

static void
maigre_rc_style_class_init (MaigreRcStyleClass *klass)
{
    GtkRcStyleClass *rc_style_class = GTK_RC_STYLE_CLASS (klass);
    rc_style_class->create_style = maigre_rc_style_create_style;
}

static void
maigre_rc_style_class_finalize (MaigreRcStyleClass *klass)
{
}
