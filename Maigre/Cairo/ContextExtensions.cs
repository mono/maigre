//
// Style.cs
//  
// Author:
//   Aaron Bockover <abockover@novell.com>
// 
// Copyright 2007-2010 Novell, Inc.
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

using System;

namespace Cairo
{
    public static class ContextExtensions
    {
        /*public static Pango.Layout CreateCairoLayout (Gtk.Widget widget, Cairo.Context cairo_context)
        {
            var layout = PangoCairoHelper.CreateLayout (cairo_context);
            layout.FontDescription = widget.PangoContext.FontDescription;

            double resolution = widget.Screen.Resolution;
            if (resolution != -1) {
                using (var context = PangoCairoHelper.LayoutGetContext (layout)) {
                    PangoCairoHelper.ContextSetResolution (context, resolution);
                }
            }

            return layout;
        }*/

        public static Surface CreateSurfaceForPixbuf (this Cairo.Context cr, Gdk.Pixbuf pixbuf)
        {
            var surface = cr.Target.CreateSimilar (cr.Target.Content, pixbuf.Width, pixbuf.Height);
            using (var surface_cr = new Context (surface)) {
                Gdk.CairoHelper.SetSourcePixbuf (surface_cr, pixbuf, 0, 0);
                surface_cr.Paint ();
                return surface;
            }
        }

        public static void RoundedRectangle (this Cairo.Context cr,
            Cairo.Rectangle rect, double r)
        {
            RoundedRectangle (cr, rect.X, rect.Y, rect.Width, rect.Height,
                r, Corner.All, false);
        }

        public static void RoundedRectangle (this Cairo.Context cr,
            Cairo.Rectangle rect, double r, Corner corners)
        {
            RoundedRectangle (cr, rect.X, rect.Y, rect.Width, rect.Height,
                r, corners, false);
        }

        public static void RoundedRectangle (this Cairo.Context cr,
            Cairo.Rectangle rect, double r, Corner corners,
            bool topBottomFallsThrough)
        {
            RoundedRectangle (cr, rect.X, rect.Y, rect.Width, rect.Height,
                r, corners, topBottomFallsThrough);
        }

        public static void RoundedRectangle (this Cairo.Context cr,
            double x, double y, double w, double h, double r)
        {
            RoundedRectangle (cr, x, y, w, h, r, Corner.All, false);
        }

        public static void RoundedRectangle (this Cairo.Context cr,
            double x, double y, double w, double h,
            double r, Corner corners)
        {
            RoundedRectangle (cr, x, y, w, h, r, corners, false);
        }

        public static void RoundedRectangle (this Cairo.Context cr,
            double x, double y, double w, double h,
            double r, Corner corners, bool topBottomFallsThrough)
        {
            if (topBottomFallsThrough && corners == Corner.None) {
                cr.MoveTo (x, y - r);
                cr.LineTo (x, y + h + r);
                cr.MoveTo (x + w, y - r);
                cr.LineTo (x + w, y + h + r);
                return;
            } else if (r < 0.0001 || corners == Corner.None) {
                cr.Rectangle (x, y, w, h);
                return;
            }

            if ((corners & (Corner.TopLeft | Corner.TopRight)) == 0 && topBottomFallsThrough) {
                y -= r;
                h += r;
                cr.MoveTo (x + w, y);
            } else {
                if ((corners & Corner.TopLeft) != 0) {
                    cr.MoveTo (x + r, y);
                } else {
                    cr.MoveTo (x, y);
                }

                if ((corners & Corner.TopRight) != 0) {
                    cr.Arc (x + w - r, y + r, r, Math.PI * 1.5, Math.PI * 2);
                } else {
                    cr.LineTo (x + w, y);
                }
            }

            if ((corners & (Corner.BottomLeft | Corner.BottomRight)) == 0 && topBottomFallsThrough) {
                h += r;
                cr.LineTo (x + w, y + h);
                cr.MoveTo (x, y + h);
                cr.LineTo (x, y + r);
                cr.Arc (x + r, y + r, r, Math.PI, Math.PI * 1.5);
            } else {
                if ((corners & Corner.BottomRight) != 0) {
                    cr.Arc (x + w - r, y + h - r, r, 0, Math.PI * 0.5);
                } else {
                    cr.LineTo (x + w, y + h);
                }

                if ((corners & Corner.BottomLeft) != 0) {
                    cr .Arc(x + r, y + h - r, r, Math.PI * 0.5, Math.PI);
                } else {
                    cr.LineTo (x, y + h);
                }

                if ((corners & Corner.TopLeft) != 0) {
                    cr.Arc (x + r, y + r, r, Math.PI, Math.PI * 1.5);
                } else {
                    cr.LineTo (x, y);
                }
            }
        }

        public static void DisposeSelfAndTarget (this Cairo.Context cr)
        {
            ((IDisposable)cr.Target).Dispose ();
            ((IDisposable)cr).Dispose ();
        }

        public static void SetColor (this Cairo.Context cr, Gdk.Color color)
        {
            cr.Color = color.ToCairoColor ();
        }

        public static void FillPatternDestroy (this Cairo.Context cr, Cairo.Pattern pattern)
        {
            FillPatternDestroy (cr, pattern, false);
        }

        public static void FillPatternDestroy (this Cairo.Context cr, Cairo.Pattern pattern, bool preserve)
        {
            cr.Pattern = pattern;
            if (preserve) {
                cr.FillPreserve ();
            } else {
                cr.Fill ();
            }
            pattern.Destroy ();
        }

        public static void Translate (this Cairo.Context cr, Cairo.Rectangle rect)
        {
            cr.Translate (rect.X, rect.Y);
        }

        public static void Translate (this Cairo.Context cr, Cairo.Point point)
        {
            cr.Translate (point.X, point.Y);
        }
    }
}
