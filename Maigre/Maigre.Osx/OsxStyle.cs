// 
// OsxStyle.cs
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
using Cairo;

namespace Maigre.Osx
{
    public class OsxStyle : Style
    {
        public OsxStyle (IntPtr raw) : base (raw)
        {
        }

        protected override void OnDrawBox ()
        {
            switch (Detail) {
                case "button": DrawButton (); break;
                default: base.OnDrawBox (); break;
            }
        }

        private void DrawButton ()
        {
            Cr.RoundedRectangle (Shape, 3);
            var grad = new LinearGradient (0, 0, 0, Height);
            grad.AddColorStop (StateType == Gtk.StateType.Active ? 1 : 0,
                ColorExtensions.FromRgb (0xeeeeee));
            grad.AddColorStop (StateType == Gtk.StateType.Active ? 0 : 1,
                ColorExtensions.FromRgb (0xcccccc));
            Cr.FillPatternDestroy (grad, true);

            Cr.RoundedRectangle (Shape.Inset (0.5), 3);
            Cr.LineWidth = 1;
            Cr.Color = ColorExtensions.FromRgb (0x999999);
            Cr.Stroke ();
        }

        protected override void OnDrawFocus ()
        {
            Cr.RoundedRectangle (Shape, 2);
            Cr.Color = ColorExtensions.FromRgba (0x00000011);
            Cr.Fill ();
            Cr.RoundedRectangle (Shape.Inset (0.5), 2);
            Cr.Color = ColorExtensions.FromRgba (0x00000005);
            Cr.Stroke ();
        }

        protected override void OnDrawFlatBox ()
        {
            Console.WriteLine (Detail);
            Cr.Rectangle (Shape);
            switch (Detail) {
                case "base":
                    Cr.Color = new Cairo.Color (0, 0, 0);
                    break;
                case "cell_even":
                case "cell_odd":
                    Cr.Color = new Cairo.Color (1, 0, 0, 0.25);
                    break;
            }
            Cr.Fill ();
        }
    }
}
