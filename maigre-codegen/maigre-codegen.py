#!/usr/bin/env python
#
# maigre-codegen.py
#
# Author:
#   Aaron Bockover <abockover@novell.com>
#
# Copyright 2010 Novell, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import sys
import re
from textwrap import TextWrapper
from mako.template import Template

def get_managed_name (native_name):
    managed_name = ''
    for part in native_name.split ('_'):
        managed_name += part.capitalize ()
    return managed_name

def get_managed_type (native_type, is_pointer):
    map = {
        'guint': 'uint',
        'gint': 'int',
        'gboolean': 'bool'
    }

    if native_type in map:
        return map[native_type]
    elif is_pointer and native_type == 'const gchar':
        return 'string'

    i = 0
    for c in native_type:
        if i > 1 and c.isupper ():
            break
        i += 1

    if i == len (native_type):
        return native_type

    return native_type[0:i] + '.' + native_type[i:]

class Argument:
    def __init__ (self, type, name, is_pointer):
        self.native_type = type
        self.native_name = name
        self.managed_type = get_managed_type (type, is_pointer)
        self.managed_name = get_managed_name (name)
        self.is_pointer = is_pointer
        self.is_custom_marshal = is_pointer and not self.managed_type == 'string'

    def to_native_string (self):
        string = self.native_type + ' '
        if self.is_pointer:
            string += '*'
        return string + self.native_name

    def to_managed_string (self, use_marshal_type = False):
        if self.is_custom_marshal and not use_marshal_type:
            return 'IntPtr ' + self.managed_name
        else:
            return self.managed_type + ' ' + self.managed_name

class Method:
    def __init__ (self, name):
        self.native_name = name
        self.managed_name = get_managed_name (name)
        self.arguments = []

    def to_native_signature (self):
        signature = []
        for arg in self.arguments:
            signature.append (arg.to_native_string ())
        return signature

    def to_managed_signature (self):
        signature = []
        for arg in self.arguments:
            signature.append (arg.to_managed_string ())
        return signature

class GtkStyleHeaderParser:
    def __init__ (self):
        self.methods = []
        self.blacklist = []
        self.context_struct = []

    def parse (self):
        last_word = None
        vmethod = None
        arg_type = None

        for word in open ('/usr/include/gtk-2.0/gtk/gtkstyle.h').read ().split ():
            if not last_word:
                last_word = word
                continue

            if last_word == 'void' and word.startswith ('(*draw_'):
                vmethod = Method (word.strip ('(*)'))
            elif vmethod:
                is_end = word.endswith (');')
                word = word.strip ('();,').strip ()
                if word == 'const':
                    arg_type = word
                    continue
                if not arg_type:
                    arg_type = word
                elif arg_type == 'const':
                    arg_type = 'const %s' % word
                else:
                    is_pointer = False
                    if word[0] == '*':
                        word = word[1:].strip ()
                        is_pointer = True
                    if word[0] == '_':
                        word = word[1:]
                    if word[-1] == '_':
                        word = word[0:-1]
                    vmethod.arguments.append (Argument (arg_type, word, is_pointer))
                    arg_type = None
                if is_end:
                    if vmethod.native_name not in self.blacklist:
                        self.methods.append (vmethod)
                    vmethod = None

            last_word = word

        self.build_context_struct ()

    def build_context_struct (self):
        self.context_struct = []
        self.context_struct.append (Argument ('const gchar', 'method', True))

        for method in self.methods:
            for arg in method.arguments:
                found = False
                for member in self.context_struct:
                    if arg.native_name == member.native_name and \
                        arg.native_type == member.native_type and \
                        arg.is_pointer == member.is_pointer:
                        found = True
                        break
                if not found:
                    self.context_struct.append (arg)

    def get_context_struct_marshallers (self):
        return filter (lambda member: member.is_custom_marshal, self.context_struct)

class CodeGenerator:
    def __init__ (self, parser):
        self.indent_size = 4
        self.parser = parser

    def indent (self, level = 0):
        s = ''
        for i in range (0, self.indent_size * level):
            s += ' '
        return s

    def wrap (self, string, indent = 0, subsequent_indent = 1, max_width = 70):
        TextWrapper.wordsep_re = re.compile (r'\s*(\,)')
        wrapper = TextWrapper (width = max_width)
        wrapped = ''
        i = 0
        for line in wrapper.wrap (string):
            line = line.strip ()
            if i > 0:
                if line[0] == ',':
                    line = line[1:].strip ()
                    wrapped += ',\n'
                else:
                    wrapped += '\n'
                wrapped += self.indent (subsequent_indent)
            wrapped += line
            i += 1
        return self.indent (indent) + wrapped.strip ()

    def generate (self, file):
        template = Template (filename = file)
        return template.render (parser = self.parser, generator = self)

parser = GtkStyleHeaderParser ()
parser.blacklist = ['draw_polygon']
parser.parse ()

generator = CodeGenerator (parser)
result = generator.generate (sys.argv[1])
with open (sys.argv[2], 'w') as out:
    out.write (result)
