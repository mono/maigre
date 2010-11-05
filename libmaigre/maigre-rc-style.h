// 
// maigre-rc-style.h
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

#ifndef MAIGRE_RC_STYLE_H
#define MAIGRE_RC_STYLE_H

#include <gtk/gtk.h>

typedef struct MaigreRcStyle MaigreRcStyle;
typedef struct MaigreRcStyleClass MaigreRcStyleClass;

#define MAIGRE_TYPE_RC_STYLE              (maigre_rc_style_get_type ())
#define MAIGRE_RC_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), MAIGRE_TYPE_RC_STYLE, MaigreRcStyle))
#define MAIGRE_RC_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MAIGRE_TYPE_RC_STYLE, MaigreRcStyleClass))
#define MAIGRE_IS_RC_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MAIGRE_TYPE_RC_STYLE))
#define MAIGRE_IS_RC_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MAIGRE_TYPE_RC_STYLE))
#define MAIGRE_RC_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MAIGRE_TYPE_RC_STYLE, MaigreRcStyleClass))

GType maigre_rc_style_get_type (void);
void  maigre_rc_style_register_types (GTypeModule *module);

#endif /* MAIGRE_RC_STYLE_H */
