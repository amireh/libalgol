#!/usr/bin/env python

import sys
import os

os.chdir(sys.argv[1] or ".")

args = ['-Wall', '-python', '-c++', '-threads']

# add -py3 option if the user is running python3
if sys.version_info.major == 3:
  args.append('-py3')

os.system("swig %s -Ipackages -o wrappers/algol_py_wrap.cxx packages/algol_py.i" % ' '.join(args))