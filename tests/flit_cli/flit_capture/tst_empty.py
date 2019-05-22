# -- LICENSE BEGIN --
#
# Copyright (c) 2015-2018, Lawrence Livermore National Security, LLC.
#
# Produced at the Lawrence Livermore National Laboratory
#
# Written by
#   Michael Bentley (mikebentley15@gmail.com),
#   Geof Sawaya (fredricflinstone@gmail.com),
#   and Ian Briggs (ian.briggs@utah.edu)
# under the direction of
#   Ganesh Gopalakrishnan
#   and Dong H. Ahn.
#
# LLNL-CODE-743137
#
# All rights reserved.
#
# This file is part of FLiT. For details, see
#   https://pruners.github.io/flit
# Please also read
#   https://github.com/PRUNERS/FLiT/blob/master/LICENSE
#
# Redistribution and use in source and binary forms, with or
# without modification, are permitted provided that the following
# conditions are met:
#
# - Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the disclaimer below.
#
# - Redistributions in binary form must reproduce the above
#   copyright notice, this list of conditions and the disclaimer
#   (as noted below) in the documentation and/or other materials
#   provided with the distribution.
#
# - Neither the name of the LLNS/LLNL nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
# CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL
# SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
#
# Additional BSD Notice
#
# 1. This notice is required to be provided under our contract
#    with the U.S. Department of Energy (DOE). This work was
#    produced at Lawrence Livermore National Laboratory under
#    Contract No. DE-AC52-07NA27344 with the DOE.
#
# 2. Neither the United States Government nor Lawrence Livermore
#    National Security, LLC nor any of their employees, makes any
#    warranty, express or implied, or assumes any liability or
#    responsibility for the accuracy, completeness, or usefulness of
#    any information, apparatus, product, or process disclosed, or
#    represents that its use would not infringe privately-owned
#    rights.
#
# 3. Also, reference herein to any specific commercial products,
#    process, or services by trade name, trademark, manufacturer or
#    otherwise does not necessarily constitute or imply its
#    endorsement, recommendation, or favoring by the United States
#    Government or Lawrence Livermore National Security, LLC. The
#    views and opinions of authors expressed herein do not
#    necessarily state or reflect those of the United States
#    Government or Lawrence Livermore National Security, LLC, and
#    shall not be used for advertising or product endorsement
#    purposes.
#
# -- LICENSE END --

'''
Tests FLiT's capture subcommand with nothing to build

The tests are below using doctest

>>> import os
>>> from io import StringIO
>>> import json
>>> import shutil

Helpers
>>> class CaptureTestError(RuntimeError): pass
>>> def run_capture(argv):
...     with th.tempdir() as temp_dir:
...         os.chdir(temp_dir)
...         with StringIO() as ostream:
...             retval = th.flit.main(argv, outstream=ostream)
...             if retval != 0:
...                 raise CaptureTestError(
...                     'could not capture "true" command (retval={0}): '
...                     .format(retval) + ostream.getvalue())
...             out = ostream.getvalue().splitlines()
...         listing = os.listdir()
...         content_map = {}
...         for fname in listing:
...             with open(fname, 'r') as fin:
...                 content_map[fname] = json.load(fin)
...     return out, content_map
>>> truepath = shutil.which('true')

No arguments and the default output filename

Note that bash has its own version of the 'true' command, so to ensure the one
from the filesystem is used, the full path must be given.

>>> out, content_map = run_capture(['capture', truepath])
>>> out
[]
>>> content_map
{'compile_commands.json': []}

No build, but specifying the output filename

>>> import tempfile
>>> with tempfile.NamedTemporaryFile() as fout:
...     fname = os.path.basename(fout.name)
...     out, content_map = run_capture(['capture', '-o', fname, truepath])

>>> out
[]
>>> len(content_map)
1
>>> content_map[fname]
[]

Capture the innocuous 'true' command as a compiler.

>>> with tempfile.NamedTemporaryFile(suffix='.cpp') as sourcefile:
...     fname = sourcefile.name
...     out, content_map = run_capture(['capture', '--cdb', 'out.json',
...                                     '--add-c++', 'true',
...                                     'bash', '-c',
...                                     truepath + ' ' + fname])
>>> out
[]
>>> len(content_map['out.json'])
1
>>> relfname = os.path.join('..', os.path.basename(fname))
>>> compilation = content_map['out.json'][0]
>>> 'directory' in compilation
True
>>> expected_compilation = {
...     'arguments': ['true', '-c', relfname],
...     'compiler': 'true',
...     'file': relfname,
...     'directory': compilation['directory'], # a temporary directory
...     'language': 'c++',
...     }
>>> compilation == expected_compilation
True
>>> os.path.dirname(compilation['directory']) == '/tmp'
True

Capture the innocuous 'true' command as a compiler, even if it is the top-level
compilation command (i.e., not wrapped in a "bash -c" call).

>>> with tempfile.NamedTemporaryFile(suffix='.cpp') as sourcefile:
...     fname = sourcefile.name
...     out, content_map = run_capture(['capture', '--cdb', 'out.json',
...                                     '--add-c++', 'true',
...                                     truepath, fname])
>>> out
[]
>>> len(content_map['out.json'])
1
>>> relfname = os.path.join('..', os.path.basename(fname))
>>> compilation = content_map['out.json'][0]
>>> 'directory' in compilation
True
>>> expected_compilation = {
...     'arguments': ['true', '-c', relfname],
...     'compiler': 'true',
...     'file': relfname,
...     'directory': compilation['directory'], # a temporary directory
...     'language': 'c++',
...     }
>>> compilation == expected_compilation
True
>>> os.path.dirname(compilation['directory']) == '/tmp'
True

'''

# Test setup before the docstring is run.
import sys
before_path = sys.path[:]
sys.path.append('../..')
import test_harness as th
sys.path = before_path

if __name__ == '__main__':
    from doctest import testmod
    failures, tests = testmod()
    sys.exit(failures)
