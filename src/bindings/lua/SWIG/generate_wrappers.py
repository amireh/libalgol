#!/usr/bin/env python

import sys
import os

os.chdir(sys.argv[1] or ".")

args = ['-Wall', '-lua', '-c++']

os.system("swig %s -Ipackages -o wrappers/lua_algol_wrap.cxx packages/lua_algol.i" % ' '.join(args))
