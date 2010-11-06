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

static MonoObject *
maigre_draw_context_new (MaigreMonoBridge *bridge, const gchar *method,
    GtkStyle *style, GdkWindow *window, GtkStateType state_type,
    GtkShadowType shadow_type, GdkRectangle *area, GtkWidget *widget,
    const gchar *detail, gint x, gint y, gint width, gint height,
    GtkOrientation orientation, GtkExpanderStyle expander_style,
    PangoLayout *layout, gboolean use_text, GdkWindowEdge edge, gint step,
    gint x1, gint x2, gint y1, gint y2, gboolean fill, GtkArrowType arrow_type)
{
    MonoObject *context = mono_object_new (bridge->domain, bridge->draw_context.klass);

    if (method != NULL) {
        mono_field_set_value (context, bridge->draw_context.method,
            mono_string_new (bridge->domain, method));
    }

    if (style != NULL) {
        mono_field_set_value (context, bridge->draw_context.style,
            maigre_gobject_new (bridge, style));
    }

    if (window != NULL) {
        mono_field_set_value (context, bridge->draw_context.window,
            maigre_gobject_new (bridge, window));
    }

    if (area != NULL) {
        mono_field_set_value (context, bridge->draw_context.area,
            maigre_gdk_rectangle_new (bridge, area));
    }

    if (widget != NULL) {
        mono_field_set_value (context, bridge->draw_context.widget,
            maigre_gobject_new (bridge, widget));
    }

    if (detail != NULL) {
        mono_field_set_value (context, bridge->draw_context.detail,
            mono_string_new (bridge->domain, detail));
    }

    mono_field_set_value (context, bridge->draw_context.state_type, &state_type);
    mono_field_set_value (context, bridge->draw_context.shadow_type, &shadow_type);
    mono_field_set_value (context, bridge->draw_context.x, &x);
    mono_field_set_value (context, bridge->draw_context.y, &y);
    mono_field_set_value (context, bridge->draw_context.width, &width);
    mono_field_set_value (context, bridge->draw_context.height, &height);
    mono_field_set_value (context, bridge->draw_context.orientation, &orientation);
    mono_field_set_value (context, bridge->draw_context.expander_style, &expander_style);

    if (layout != NULL) {
        mono_field_set_value (context, bridge->draw_context.layout,
            maigre_gobject_new (bridge, layout));
    }

    mono_field_set_value (context, bridge->draw_context.use_text, &use_text);
    mono_field_set_value (context, bridge->draw_context.edge, &edge);
    mono_field_set_value (context, bridge->draw_context.step, &step);
    mono_field_set_value (context, bridge->draw_context.x1, &x1);
    mono_field_set_value (context, bridge->draw_context.x2, &x2);
    mono_field_set_value (context, bridge->draw_context.y1, &y1);
    mono_field_set_value (context, bridge->draw_context.y2, &y2);
    mono_field_set_value (context, bridge->draw_context.fill, &fill);
    mono_field_set_value (context, bridge->draw_context.arrow_type, &arrow_type);

    return mono_object_unbox (context);
}

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
                1);
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