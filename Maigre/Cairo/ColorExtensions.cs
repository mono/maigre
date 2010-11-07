// 
// ColorExtensions.cs
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

namespace Cairo
{
    public static class ColorExtensions
    {
        public static Cairo.Color ToCairoColor (this Gdk.Color color)
        {
            return ToCairoColor (color, 1.0);
        }

        public static Cairo.Color ToCairoColor (this Gdk.Color color, double alpha)
        {
            return new Cairo.Color (
                (double)(color.Red >> 8) / 255.0,
                (double)(color.Green >> 8) / 255.0,
                (double)(color.Blue >> 8) / 255.0,
                alpha);
        }

        public static Cairo.Color AlphaBlend (this Cairo.Color ca, Cairo.Color cb, double alpha)
        {
            return new Cairo.Color (
                (1.0 - alpha) * ca.R + alpha * cb.R,
                (1.0 - alpha) * ca.G + alpha * cb.G,
                (1.0 - alpha) * ca.B + alpha * cb.B);
        }

        public static Cairo.Color FromRgb (uint rgbColor)
        {
            return FromRgba ((rgbColor << 8) | 0x000000ff);
        }

        public static Cairo.Color FromRgba (uint rgbaColor)
        {
            return new Cairo.Color (
                (byte)(rgbaColor >> 24) / 255.0,
                (byte)(rgbaColor >> 16) / 255.0,
                (byte)(rgbaColor >> 8) / 255.0,
                (byte)(rgbaColor & 0x000000ff) / 255.0);
        }

        public static bool ColorIsDark (this Cairo.Color color)
        {
            double h, s, b;
            ToHsb (color, out h, out s, out b);
            return b < 0.5;
        }

        public static void ToHsb (this Cairo.Color color, out double hue,
            out double saturation, out double brightness)
        {
            double min, max, delta;
            double red = color.R;
            double green = color.G;
            double blue = color.B;

            hue = 0;
            saturation = 0;
            brightness = 0;

            if (red > green) {
                max = Math.Max (red, blue);
                min = Math.Min (green, blue);
            } else {
                max = Math.Max (green, blue);
                min = Math.Min (red, blue);
            }

            brightness = (max + min) / 2;

            if (Math.Abs (max - min) < 0.0001) {
                hue = 0;
                saturation = 0;
            } else {
                saturation = brightness <= 0.5
                    ? (max - min) / (max + min)
                    : (max - min) / (2 - max - min);

                delta = max - min;

                if (red == max) {
                    hue = (green - blue) / delta;
                } else if (green == max) {
                    hue = 2 + (blue - red) / delta;
                } else if (blue == max) {
                    hue = 4 + (red - green) / delta;
                }

                hue *= 60;
                if (hue < 0) {
                    hue += 360;
                }
            }
        }

        private static double Modula (double number, double divisor)
        {
            return ((int)number % divisor) + (number - (int)number);
        }

        public static Cairo.Color FromHsb (double hue, double saturation, double brightness)
        {
            int i;
            double [] hue_shift = { 0, 0, 0 };
            double [] color_shift = { 0, 0, 0 };
            double m1, m2, m3;

            m2 = brightness <= 0.5
                ? brightness * (1 + saturation)
                : brightness + saturation - brightness * saturation;

            m1 = 2 * brightness - m2;

            hue_shift[0] = hue + 120;
            hue_shift[1] = hue;
            hue_shift[2] = hue - 120;

            color_shift[0] = color_shift[1] = color_shift[2] = brightness;

            i = saturation == 0 ? 3 : 0;

            for (; i < 3; i++) {
                m3 = hue_shift[i];

                if (m3 > 360) {
                    m3 = Modula (m3, 360);
                } else if(m3 < 0) {
                    m3 = 360 - Modula (Math.Abs (m3), 360);
                }

                if (m3 < 60) {
                    color_shift[i] = m1 + (m2 - m1) * m3 / 60;
                } else if (m3 < 180) {
                    color_shift[i] = m2;
                } else if (m3 < 240) {
                    color_shift[i] = m1 + (m2 - m1) * (240 - m3) / 60;
                } else {
                    color_shift[i] = m1;
                }
            }

            return new Cairo.Color (color_shift[0], color_shift[1], color_shift[2]);
        }

        public static Cairo.Color Shade (this Cairo.Color @base, double ratio)
        {
            double h, s, b;

            ToHsb (@base, out h, out s, out b);

            b = Math.Max (Math.Min (b * ratio, 1), 0);
            s = Math.Max (Math.Min (s * ratio, 1), 0);

            var color = FromHsb (h, s, b);
            color.A = @base.A;
            return color;
        }

        public static Cairo.Color AdjustBrightness (this Cairo.Color @base, double br)
        {
            double h, s, b;
            ToHsb (@base, out h, out s, out b);
            b = Math.Max (Math.Min (br, 1), 0);
            return FromHsb (h, s, b);
        }

        public static string GetHexString (this Cairo.Color color, bool withAlpha)
        {
            return withAlpha
                ? String.Format ("#{0:x2}{1:x2}{2:x2}{3:x2}",
                    (byte)(color.R * 255),
                    (byte)(color.G * 255),
                    (byte)(color.B * 255),
                    (byte)(color.A * 255))
                : String.Format ("#{0:x2}{1:x2}{2:x2}",
                    (byte)(color.R * 255),
                    (byte)(color.G * 255),
                    (byte)(color.B * 255));
        }
    }
}
