// 
// Style.cs
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

using System;
using Gtk;
using Gdk;
using Cairo;

namespace Maigre
{
    public static class Theme
    {
        public static void DrawBox (Gtk.Style style, Gdk.Window window,
            StateType state_type, ShadowType shadow_type, Gdk.Rectangle area, Widget widget,
            string detail, int x, int y, int width, int height)
        {
            var cr = Gdk.CairoHelper.Create (widget.GdkWindow);
            switch (detail) {
                case "hscrollbar":
                case "vscrollbar": cr.Color = new Cairo.Color (1, 0, 0); break;
                case "slider": cr.Color = new Cairo.Color (0, 1, 0); break;
                case "trough": cr.Color = new Cairo.Color (0, 0, 1); break;
            }
            cr.Rectangle (x, y, width, height);
            cr.Fill ();
            ((IDisposable)cr).Dispose ();
        }
    }
}
