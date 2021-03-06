m4_dnl
m4_dnl Miscellaneous m4 macros.
m4_dnl
m4_dnl Copyright (C) 2016 Red Hat
m4_dnl
m4_dnl This file is part of tlog.
m4_dnl
m4_dnl Tlog is free software; you can redistribute it and/or modify
m4_dnl it under the terms of the GNU General Public License as published by
m4_dnl the Free Software Foundation; either version 2 of the License, or
m4_dnl (at your option) any later version.
m4_dnl
m4_dnl Tlog is distributed in the hope that it will be useful,
m4_dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
m4_dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
m4_dnl GNU General Public License for more details.
m4_dnl
m4_dnl You should have received a copy of the GNU General Public License
m4_dnl along with tlog; if not, write to the Free Software
m4_dnl Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
m4_dnl
m4_dnl
m4_dnl Concatenate arguments into single string
m4_dnl
m4_define(`m4_concat',
          `m4_ifelse(`$#', `0', ,
                     `$#', `1', `$1',
                     `$1`'m4_concat(m4_shift($@))')')m4_dnl
m4_dnl
m4_dnl
m4_dnl Append arguments with newlines and concatenate into a single string.
m4_dnl
m4_define(`m4_concatl',
          `m4_ifelse(`$#', `0', ,
                     `$#', `1', `$1
',
                     `$1
m4_concatl(m4_shift($@))')')m4_dnl
m4_dnl
m4_dnl
m4_dnl Concatenate arguments into diversion 0
m4_dnl Arguments:
m4_dnl
m4_dnl      $@  Text to output.
m4_dnl
m4_define(`m4_print', `m4_dnl
m4_pushdef(`m4_orig_divnum', m4_divnum)m4_dnl
m4_divert(0)m4_dnl
m4_concat($@)`'m4_dnl
m4_divert(m4_orig_divnum)m4_dnl
m4_popdef(`m4_orig_divnum')m4_dnl
')m4_dnl
m4_dnl
m4_dnl
m4_dnl Append arguments with newlines and concatenate into diversion 0.
m4_dnl Arguments:
m4_dnl
m4_dnl      $@  Text to output.
m4_dnl
m4_define(`m4_printl', `m4_dnl
m4_pushdef(`m4_orig_divnum', m4_divnum)m4_dnl
m4_divert(0)m4_dnl
m4_concatl($@)`'m4_dnl
m4_divert(m4_orig_divnum)m4_dnl
m4_popdef(`m4_orig_divnum')m4_dnl
')m4_dnl
m4_dnl
m4_dnl
m4_dnl Output a warning header about the file being generated.
m4_dnl Arguments:
m4_dnl
m4_dnl      $1  Prefix to put in front of each line.
m4_dnl
m4_define(
`m4_generated_warning',
`m4_concatl(
`$1vim:nomodifiable',
`$1',
`$1************************* WARNING! DO NOT EDIT! *************************',
`$1This file is automatically generated from m4___file__.',
`$1*************************************************************************')')m4_dnl
m4_dnl
m4_dnl
m4_dnl Quote the argument in single quotes, as a C character literal.
m4_dnl Arguments:
m4_dnl
m4_dnl      $1  The text to single-quote.
m4_dnl
m4_changequote(`[', `]')m4_dnl
m4_define([m4_singlequote], ['$1'])m4_dnl
m4_changequote([`], ['])m4_dnl
m4_dnl
m4_dnl Convert all argument letters to upper case.
m4_dnl Arguments:
m4_dnl
m4_dnl      $1  The text to upper-case.
m4_dnl
m4_define(`m4_upcase', `m4_translit(`$1', `a-z', `A-Z')')m4_dnl
m4_dnl
m4_dnl Shift N arguments.
m4_dnl Arguments:
m4_dnl
m4_dnl      $1      Number of arguments to shift.
m4_dnl      $@      The arguments to shift.
m4_dnl
m4_define(`m4_shiftn',
          `m4_ifelse(`$#', `0', ,
                     `$#', `1', ,
                     `m4_ifelse(m4_eval(`$1'), `', `m4_shift($@)',
                                m4_eval(`$1'), `0', `m4_shift($@)',
                                `m4_shiftn(m4_decr(`$1'), m4_shift(m4_shift($@)))')')')m4_dnl
m4_dnl
m4_dnl Expand to 1+Nth argument.
m4_dnl Arguments:
m4_dnl
m4_dnl      $1      Number of the argument to expand to, minus one.
m4_dnl      $@      Arguments to expand from.
m4_dnl
m4_define(`m4_argn',
          `m4_ifelse(`$1', `1', `$2',
                     `m4_argn(m4_decr(`$1'), m4_shift(m4_shift($@)))')')m4_dnl
m4_dnl
m4_dnl Replace specific strings with another string, otherwise don't modify.
m4_dnl Arguments:
m4_dnl
m4_dnl      $1      The string to check and replace
m4_dnl      $N*2    The Nth string to check against
m4_dnl      $N*2+1  The Nth string replacement
m4_dnl
m4_define(`m4_repl_cases',
          `m4_ifelse(`$#', `0', ,
                     `$#', `1', `$1',
                     `$#', `2', `m4_ifelse(`$1', `$2', `', `$1')',
                     `m4_ifelse(`$1', `$2', `$3',
                                `m4_repl_cases(`$1', m4_shiftn(3, $@))')')')m4_dnl
m4_dnl
m4_dnl Substitute a string's substrings matching a list of regular expressions
m4_dnl with corresponding replacements.
m4_dnl Arguments:
m4_dnl
m4_dnl      $1      The string to match and replace substrings in.
m4_dnl      $N*2    The Nth regular expression to match against.
m4_dnl      $N*2+1  The Nth replacement.
m4_dnl
m4_define(`m4_patsubst_list',
          `m4_ifelse(`$#', `0', ,
                     `$#', `1', `$1',
                     `$#', `2', `m4_patsubst(`$1', `$2')',
                     `$#', `3', `m4_patsubst(`$1', `$2', `$3')',
                     `m4_patsubst_list(m4_patsubst(`$1', `$2', `$3'),
                                       m4_shiftn(3, $@))')')m4_dnl
m4_dnl
m4_dnl Escape string for use in C string literal.
m4_dnl Arguments:
m4_dnl
m4_dnl      $1      The string to escape.
m4_dnl
m4_define(`m4_cstr_esc',
          `m4_patsubst_list(`$1',
                            `[\"?]', `\\\&',
                            m4_format(`%c',  1), `\\x01',
                            m4_format(`%c',  2), `\\x02',
                            m4_format(`%c',  3), `\\x03',
                            m4_format(`%c',  4), `\\x04',
                            m4_format(`%c',  5), `\\x05',
                            m4_format(`%c',  6), `\\x06',
                            m4_format(`%c',  7), `\\a',
                            m4_format(`%c',  8), `\\b',
                            m4_format(`%c',  9), `\\t',
                            m4_format(`%c', 10), `\\n',
                            m4_format(`%c', 11), `\\v',
                            m4_format(`%c', 12), `\\f',
                            m4_format(`%c', 13), `\\r',
                            m4_format(`%c', 14), `\\x0e',
                            m4_format(`%c', 15), `\\x0f',
                            m4_format(`%c', 16), `\\x10',
                            m4_format(`%c', 17), `\\x11',
                            m4_format(`%c', 18), `\\x12',
                            m4_format(`%c', 19), `\\x13',
                            m4_format(`%c', 20), `\\x14',
                            m4_format(`%c', 21), `\\x15',
                            m4_format(`%c', 22), `\\x16',
                            m4_format(`%c', 23), `\\x17',
                            m4_format(`%c', 24), `\\x18',
                            m4_format(`%c', 25), `\\x19',
                            m4_format(`%c', 26), `\\x1a',
                            m4_format(`%c', 27), `\\x1b',
                            m4_format(`%c', 28), `\\x1c',
                            m4_format(`%c', 29), `\\x1d',
                            m4_format(`%c', 30), `\\x1e',
                            m4_format(`%c', 31), `\\x1f')')m4_dnl
