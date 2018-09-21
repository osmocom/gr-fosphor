#!/usr/bin/env python3

"""
Packs a given file list into a .c file to be included in the executable
directly.


Copyright (C) 2013-2014 Sylvain Munaut

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import binascii
import sys


class ResourcePacker(object):

	def header(self):
		return [
			"/* AUTO GENERATED - DO NOT MODIFY BY HAND */",
			"#include \"resource_internal.h\"",
		]

	def rp_header(self):
		return [
			"struct resource_pack __resources[] = {",
		]

	def _file_wrap(self, name, len_, data_lines):
		a = [
			"\t{",
			"\t\t.name = \"%s\"," % name,
			"\t\t.len  = %d," % len_,
		]

		if len(data_lines) > 1:
			b = [ "\t\t.data =" ]
			for l in data_lines:
				b.append("\t\t\t" + l)
			b[-1] += ","
		else:
			b = [ "\t\t.data = %s," % data_lines[0] ]

		c = [
			"\t},",
		]

		return a+b+c

	def _wrap_str(self, s):
		m = {
			ord(b'\n') : '\\n',
			ord(b'\t') : '\\t',
			ord(b'\0') : '\\0',
			ord(b'\\') : '\\\\',
			ord(b'"')  : '\\"',
		}

		sa = []
		for c in s:
			if c in m:
				sa.append(m[c])
			elif (c < 32) | (c >= 128):
				sa.append('\\x%02x' % c)
			else:
				sa.append(chr(c))

		return ''.join(sa)

	def file_text(self, name, content):
		dl = ['\"' + self._wrap_str(l) + '\"' for l in content.splitlines(True)]
		return self._file_wrap(name, len(content), dl)

	def file_binary(self, name, content):
		return self._file_wrap(name, len(content), [ self.file_binary_slug(name) ])

	def rp_footer(self):
		return [
			"\t/* Guard */",
			"\t{ .name = (void*)0 }",
			"};",
		]

	def file_binary_slug(self, name):
		return 'data_' + binascii.hexlify(name.encode('utf-8')).decode('utf-8')

	def file_binary_data(self, name, content):
		dl = [ 'static const char %s[] = {' % self.file_binary_slug(name) ]
		for i in range(0, len(content), 8):
			dl.append('\t' + ' '.join(['0x%02x,' % c for c in content[i:i+8]]))
		dl.append('};');
		return dl

	def process(self, filenames):
		b = []
		b.extend(self.header())

		data = []
		rp = []
		rp.extend(self.rp_header())
		for f in filenames:
			fh = open(f, 'rb')
			c = fh.read()
			if b'\x00' in c or len(c) > 65535:
				rp.extend(self.file_binary(f, c))
				data.extend(self.file_binary_data(f, c))
			else:
				rp.extend(self.file_text(f, c))
			fh.close()
		rp.extend(self.rp_footer())

		b.extend(data)
		b.extend(rp)

		return b;


def main(self, *files):
	p = ResourcePacker()
	r = p.process(files)
	print('\n'.join(r))


if __name__ == '__main__':
	main(*sys.argv)
