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
    public struct DrawContext
    {
        public string Method { get; private set; }
        public Gtk.Style Style { get; private set; }
        public Gdk.Window Window { get; private set; }
        public Gtk.StateType StateType { get; private set; }
        public Gtk.ShadowType ShadowType { get; private set; }
        public Gdk.Rectangle Area { get; private set; }
        public Gtk.Widget Widget { get; private set; }
        public string Detail { get; private set; }
        public int X { get; private set; }
        public int Y { get; private set; }
        public int Width { get; private set; }
        public int Height { get; private set; }
        public Gtk.Orientation Orientation { get; private set; }
        public Gtk.ExpanderStyle ExpanderStyle { get; private set; }
        public Pango.Layout Layout { get; private set; }
        public bool UseText { get; private set; }
        public Gdk.WindowEdge Edge { get; private set; }
        public int Step { get; private set; }
        public int X1 { get; private set; }
        public int X2 { get; private set; }
        public int Y1 { get; private set; }
        public int Y2 { get; private set; }
        public bool Fill { get; private set; }
        public Gtk.ArrowType ArrowType { get; private set; }
    }

    public static class Theme
    {
        public static void DrawBox (DrawContext context)
        {
            Console.WriteLine ("{0}:{1} ({2})", context.Method, context.Detail, context.Area);
            using (var cr = Gdk.CairoHelper.Create (context.Window)) {
                cr.Color = new Cairo.Color (0, 1, 1);
                cr.Rectangle (context.X, context.Y, context.Width, context.Height);
                cr.Fill ();
            }
        }
    }
}
